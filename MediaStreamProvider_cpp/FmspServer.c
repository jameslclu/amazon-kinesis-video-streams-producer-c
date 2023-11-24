/**
 * Copyright (c) 2022 D Group / PCEBG / SHPBU, Foxconn .
 * All rights reserved.
 */

/**
 * @ingroup FXN-MEDIA-STREAMING-PROVIDER-SERVER
 * @file    FmspServer.c
 * @brief   Source files for Media provider server.
 */

/* ---- Includes ----------------------------------------------------------- */
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <json-c/json.h>

#include "LibFxnMediaProviderDef.h"
#include "LibFxnDBus.h"

#include "inc/FmspServer.h"
#include "inc/OryxStreaming.h"
#include "inc/FmspCommon.h"
//#define LIVENEABLE 1
//#define PLAYBACKENABLE 1
/* ---- Constants ---------------------------------------------------------- */

/* ---- Types -------------------------------------------------------------- */
typedef struct {
    int32_t Width;
    int32_t Height;
}FmspVideoResolution_t;

typedef struct{
    const FmspVideoResolution_t TblResolution[ LIB_DBUS_CLIENT_CAMERA_RESOLUTION_MAX ];

    bool bChanged;
    bool Reserved[ 3 ];

    int32_t Codec;
    int32_t Framerate;
    int32_t TargetBitrate;
    int32_t ResolutionIndex;
}FmspServerMediaConfig_t;

typedef struct{
    FmspServer_t Playback;
    FmspServer_t Liveview;

    FmspServerMediaConfig_t Config;
    LibDBusClientCallbackFn_t SignalCb;
}FmspServerConfig_t;

/* ---- Private Function Prototypes ---------------------------------------- */
/*! ------------------------------------------------
 *    Playback Server
 *  ------------------------------------------------ */
static int32_t FmspServerPlaybackCreate( void );
static void FmspServerPlaybackDestroy( void );

static void FmspServerPlaybackRecvSuspend( void );
static void FmspServerPlaybackSendSuspend( void );

static void FmspServerPlaybackRecvWakeUp( void );
static void FmspServerPlaybackSendWakeUp( void );

/*! ------------------------------------------------
 *    Liveview Server
 *  ------------------------------------------------ */
static int32_t FmspServerLiveviewCreate( void );
static void FmspServerLiveviewDestroy( void );

static void FmspServerLiveviewRecvSuspend( void );
static void FmspServerLiveviewSendSuspend( void );

static void FmspServerLiveviewRecvWakeUp( void );
static void FmspServerLiveviewSendWakeUp( void );

/*! ------------------------------------------------
 *    Common APIs
 *  ------------------------------------------------ */
static int32_t FmspServerConstruct(
    FmspServer_t * p, const char SocketPath[] );
static void FmspServerDestruct(
    FmspServer_t * p, const char SocketPath[] );

static int32_t FmspServerSocketCreate( const char * pName, FmspServer_t * p );
static void FmspServerSocketDestroy( const char * pName, FmspServer_t * p );
static int32_t FmspServerAccept(
    const int32_t ListenFd, int32_t * pClientSd, pid_t * pPid );

static void FmspServerCameraSignalCb( const char * pBuffer,
                                      const size_t Size );

/*! ------------------------------------------------
 *    Playback Server Callback
 *  ------------------------------------------------ */
static void * FmspServerPlaybackMainLoop( void * arg );
static void * FmspServerPlaybackRecvLoop( void * arg );
static void * FmspServerPlaybackSendLoop( void * arg );

/*! ------------------------------------------------
 *    Liveview Server Callback
 *  ------------------------------------------------ */
static void * FmspServerLiveviewMainLoop( void * arg );
static void * FmspServerLiveviewRecvLoop( void * arg );
static void * FmspServerLiveviewSendLoop( void * arg );

/*! ------------------------------------------------
 *    Common Callback
 *  ------------------------------------------------ */

static void FmspServerGetVideoConfiguration( void );

/* ---- Variables ---------------------------------------------------------- */
static FmspServerConfig_t gServerConfig = {
    .Config.TblResolution = {
        [ LIB_DBUS_CLIENT_CAMERA_RESOLUTION_1920X2560 ] = { 1920, 2560 },
        [ LIB_DBUS_CLIENT_CAMERA_RESOLUTION_1440X1920 ] = { 1440, 1920 },
        [ LIB_DBUS_CLIENT_CAMERA_RESOLUTION_960X1280  ] = {  960, 1280 },
        [ LIB_DBUS_CLIENT_CAMERA_RESOLUTION_480X640   ] = {  480,  640 }
    }
};

/* ---- Global Functions --------------------------------------------------- */
int32_t FmspServerCreate( void )
{
    FmspServerConfig_t * p;

    p = &gServerConfig;
    ( void )memset( &p->Playback, 0x00, sizeof( FmspServer_t ) );
#ifdef LIVENENABLE
    ( void )memset( &p->Liveview, 0x00, sizeof( FmspServer_t ) );
#endif

    p->SignalCb.Camera.onCameraConfigChanged   = &FmspServerCameraSignalCb;
    p->SignalCb.Camera.onSpotlightStateChanged = NULL;
    p->SignalCb.Camera.onIRLedStateChanged     = NULL;

    ( void )LibDBusClientCameraLink( &p->SignalCb );

    FmspServerGetVideoConfiguration();

    p = NULL;
#ifdef LIVENENABLE
    ( void )FmspServerLiveviewCreate();
#endif
    return FmspServerPlaybackCreate();
}

/* ----------------------------------------------------------- */
void FmspServerDestroy( void )
{
    FmspServerPlaybackDestroy();
#ifdef LIVENENABLE
    FmspServerLiveviewDestroy();
#endif

    ( void )memset( &gServerConfig, 0x00, sizeof( FmspServerConfig_t ) );
}

/* ---- Private Function --------------------------------------------------- */
/*! ------------------------------------------------
 *    Playback Server
 *  ------------------------------------------------ */
static int32_t FmspServerPlaybackCreate( void )
{
    FmspServer_t * p;
    int32_t      retStatus;

    retStatus = EXIT_SUCCESS;

    p = &gServerConfig.Playback;
    p->Main.ppFnLoop   = FmspServerPlaybackMainLoop;
    p->Main.pFnSuspend = NULL;
    p->Main.pFnWakup   = NULL;

    p->Recv.ppFnLoop   = FmspServerPlaybackRecvLoop;
    p->Recv.pFnSuspend = &FmspServerPlaybackRecvSuspend;
    p->Recv.pFnWakup   = &FmspServerPlaybackRecvWakeUp;

    p->Send.ppFnLoop   = FmspServerPlaybackSendLoop;
    p->Send.pFnSuspend = &FmspServerPlaybackSendSuspend;
    p->Send.pFnWakup   = &FmspServerPlaybackSendWakeUp;

    retStatus = FmspServerConstruct( p, LIB_FMSP_PLAYBACK_PATH );

    p = NULL;

    return retStatus;
}

/* ----------------------------------------------------------- */
static void FmspServerPlaybackDestroy( void )
{
    FmspServerDestruct( &gServerConfig.Playback, LIB_FMSP_PLAYBACK_PATH );
}

/* ----------------------------------------------------------- */
static void FmspServerPlaybackRecvSuspend( void )
{
    FmspServerThread_t * p;

    p = &gServerConfig.Playback.Recv;

    pthread_cleanup_push( ( void * )pthread_mutex_unlock,
                          ( void * )( &p->M ) );
    pthread_mutex_lock( &p->M );
    pthread_cond_wait( &p->C, &p->M );
    pthread_mutex_unlock( &p->M );

    pthread_cleanup_pop( 0 );

    p = NULL;
}

/* ----------------------------------------------------------- */
static void FmspServerPlaybackSendSuspend( void )
{
    FmspServerThread_t * p;

    p = &gServerConfig.Playback.Send;

    pthread_cleanup_push( ( void * )pthread_mutex_unlock,
                          ( void * )( &p->M ) );
    pthread_mutex_lock( &p->M );
    pthread_cond_wait( &p->C, &p->M );
    pthread_mutex_unlock( &p->M );

    pthread_cleanup_pop( 0 );

    p = NULL;
}

/* ----------------------------------------------------------- */
static void FmspServerPlaybackRecvWakeUp( void )
{
    pthread_cond_signal( &gServerConfig.Playback.Recv.C );
}

/* ----------------------------------------------------------- */
static void FmspServerPlaybackSendWakeUp( void )
{
    pthread_cond_signal( &gServerConfig.Playback.Send.C );
}

/*! ------------------------------------------------
 *    Liveview Server
 *  ------------------------------------------------ */
static int32_t FmspServerLiveviewCreate( void )
{
    FmspServer_t * p;
    int32_t      retStatus;

    retStatus = EXIT_SUCCESS;

    p = &gServerConfig.Liveview;

    p->Main.ppFnLoop   = FmspServerLiveviewMainLoop;
    p->Main.pFnSuspend = NULL;
    p->Main.pFnWakup   = NULL;

    p->Recv.ppFnLoop   = FmspServerLiveviewRecvLoop;
    p->Recv.pFnSuspend = &FmspServerLiveviewRecvSuspend;
    p->Recv.pFnWakup   = &FmspServerLiveviewRecvWakeUp;

    p->Send.ppFnLoop   = FmspServerLiveviewSendLoop;
    p->Send.pFnSuspend = &FmspServerLiveviewSendSuspend;
    p->Send.pFnWakup   = &FmspServerLiveviewSendWakeUp;

    retStatus = FmspServerConstruct( p, LIB_FMSP_LIVEVIEW_PATH );

    p = NULL;

    return retStatus;
}

/* ----------------------------------------------------------- */
static void FmspServerLiveviewDestroy( void )
{
    FmspServerDestruct( &gServerConfig.Liveview, LIB_FMSP_LIVEVIEW_PATH );
}

/* ----------------------------------------------------------- */
static void FmspServerLiveviewRecvSuspend( void )
{
    FmspServerThread_t * p;

    p = &gServerConfig.Liveview.Recv;

    pthread_cleanup_push( ( void * )pthread_mutex_unlock,
                          ( void * )( &p->M ) );
    pthread_mutex_lock( &p->M );
    pthread_cond_wait( &p->C, &p->M );
    pthread_mutex_unlock( &p->M );

    pthread_cleanup_pop( 0 );

    p = NULL;
}

/* ----------------------------------------------------------- */
static void FmspServerLiveviewSendSuspend( void )
{
    FmspServerThread_t * p;

    p = &gServerConfig.Liveview.Send;

    pthread_cleanup_push( ( void * )pthread_mutex_unlock,
                          ( void * )( &p->M ) );
    pthread_mutex_lock( &p->M );
    pthread_cond_wait( &p->C, &p->M );
    pthread_mutex_unlock( &p->M );

    pthread_cleanup_pop( 0 );

    p = NULL;
}

/* ----------------------------------------------------------- */
static void FmspServerLiveviewRecvWakeUp( void )
{
    pthread_cond_signal( &gServerConfig.Liveview.Recv.C );
}

/* ----------------------------------------------------------- */
static void FmspServerLiveviewSendWakeUp( void )
{
    pthread_cond_signal( &gServerConfig.Liveview.Send.C );
}

/*! ------------------------------------------------
 *    Common APIs
 *  ------------------------------------------------ */
static int32_t FmspServerConstruct(
    FmspServer_t * p, const char SocketPath[] )
{
    int32_t      retStatus;

    p->ServerSd      = -1;
    p->ClientSd[ 0 ] = -1;
    p->ClientSd[ 1 ] = -1;
    p->Recv.bRun     = false;
    p->Recv.bSuspend = true;
    p->Send.bRun     = false;
    p->Send.bSuspend = true;
    p->Send.bRenew   = false;
    p->TargetTs      = 0L;
    p->ArrivalTs     = 0L;

    retStatus = FmspServerSocketCreate( SocketPath, p );

    if( EXIT_SUCCESS == retStatus )
    {
        fcntl( p->ServerSd, F_SETFL, O_NDELAY );
        p->Main.bRun = true;
        if( 0 != pthread_create( &p->Main.TID, NULL, p->Main.ppFnLoop, NULL ) )
        {
            p->Main.bRun = false;
            retStatus = EXIT_FAILURE;
        }
    }

    if( EXIT_SUCCESS == retStatus )
    {
        /*!< Create Server Main Thread Loop For Receive Client Command Interface */
        p->Recv.bRun = true;
        if( 0 != pthread_create( &p->Recv.TID, NULL, p->Recv.ppFnLoop, NULL ) )
        {
            p->Recv.bRun = false;
            retStatus = EXIT_FAILURE;
        }
    }

    if( EXIT_SUCCESS == retStatus )
    {
        /*!< Create Server Main Thread Loop For Send Client Command Interface */
        p->Send.bRun = true;
        if( 0 != pthread_create( &p->Send.TID, NULL, p->Send.ppFnLoop, NULL ) )
        {
            p->Send.bRun = false;
            retStatus = EXIT_FAILURE;
        }
    }

    return retStatus;
}

/* ----------------------------------------------------------- */
static void FmspServerDestruct(
    FmspServer_t * p, const char SocketPath[] )
{
    if( true == p->Send.bRun )
    {
        p->Send.bRun     = false;
        p->Send.bSuspend = false;
        p->Send.bRenew   = false;
        pthread_cond_signal( &p->Send.C );
        pthread_join( p->Send.TID, NULL );

        pthread_mutex_destroy( &p->Send.M );
        pthread_cond_destroy( &p->Send.C );
    }

    /*!< Disable Receive Playback Command From Client */
    if( true == p->Recv.bRun )
    {
        p->Recv.bRun     = false;
        p->Recv.bSuspend = false;
        pthread_cond_signal( &p->Recv.C );
        pthread_join( p->Recv.TID, NULL );

        pthread_mutex_destroy( &p->Recv.M );
        pthread_cond_destroy( &p->Recv.C );
    }

    /*!< Close Playback Connection Interface */
    if( true == p->Main.bRun )
    {
        p->Main.bRun     = false;
        p->Main.bSuspend = false;
        pthread_cond_signal( &p->Main.C );
        pthread_join( p->Main.TID, NULL );

        pthread_mutex_destroy( &p->Main.M );
        pthread_cond_destroy( &p->Main.C );
    }

    FmspServerSocketDestroy( &SocketPath[ 0 ], p );
}

/* ----------------------------------------------------------- */
static int32_t FmspServerSocketCreate( const char * pName, FmspServer_t * p )
{
    struct sockaddr_un un_addr;
    int32_t retStatus;
    int32_t Length;

    retStatus = EXIT_SUCCESS;
    Length    = 0;

    p->ServerSd = socket( PF_LOCAL, SOCK_STREAM, 0 );
    if( 0 > p->ServerSd )
        retStatus = EXIT_FAILURE;

    if( EXIT_SUCCESS == retStatus )
    {
        unlink( pName );
        ( void )memset( &un_addr, 0, sizeof( un_addr ) );

        un_addr.sun_family = PF_LOCAL;
        ( void )strcpy( un_addr.sun_path, pName );
        Length = offsetof( struct sockaddr_un, sun_path ) + strlen( pName );

        if( 0 > bind( p->ServerSd, ( struct sockaddr * )&un_addr, Length ) )
            retStatus = EXIT_FAILURE;
    }

    if( EXIT_SUCCESS == retStatus )
    {
        if( 0 > listen( p->ServerSd, 2 ) )
            retStatus = EXIT_FAILURE;
    }

    p = NULL;

    return retStatus;
}

/* ----------------------------------------------------------- */
static void FmspServerSocketDestroy( const char * pName, FmspServer_t * p )
{
    if( 0 <= p->ServerSd )
        close( p->ServerSd );

    unlink( pName );

    p->ServerSd = -1;
}

/* ----------------------------------------------------------- */
static int32_t FmspServerAccept(
    const int32_t ListenFd, int32_t * pClientSd, pid_t * pPid )
{
    struct sockaddr_un un_addr;
    struct stat statbuf;
    const char * pStrPid;

    int32_t retStatus;
    int32_t ClientSd;
    int32_t Length = 0;

    retStatus = EXIT_SUCCESS;
    ClientSd = -1;
    Length = sizeof( struct sockaddr_un );
    ClientSd = accept( ListenFd,
                       ( struct sockaddr * )&un_addr,
                       ( socklen_t * )&Length );

    if( 0 > ClientSd )
        retStatus = EXIT_FAILURE;

    if( EXIT_SUCCESS == retStatus )
    {
        Length -= offsetof( struct sockaddr_un, sun_path );
        un_addr.sun_path[ Length ] = '\0';

        if( 0 > stat( un_addr.sun_path, &statbuf ) )
           retStatus = EXIT_FAILURE;
    }

    if( EXIT_SUCCESS == retStatus )
    {
        /* get pid of client from sun_path */
        pStrPid = strrchr( un_addr.sun_path, '/' );
        pStrPid++;

        * pPid = atoi( pStrPid );
        * pClientSd = ClientSd;

        unlink( un_addr.sun_path );
    }
    else
    {
        * pPid = -1;
        * pClientSd = -1;
    }

    return retStatus;
}

/* ---- Call-back Functions ------------------------------------------------ */
static void FmspServerCameraSignalCb( const char * pBuffer,
                                      const size_t Size )
{
    FXNLOGD( "Signal Trigger Data( %d ): \"%s\"", Size, pBuffer );
    gServerConfig.Config.bChanged = true;
}

/*! ------------------------------------------------
 *    Playback Server Callback
 *  ------------------------------------------------ */
static void * FmspServerPlaybackMainLoop( void * arg )
{
    FmspServer_t   * p;
    struct timeval Timeout;
    fd_set         SocketSet;
    pid_t          Pid;
    int32_t        Length;
    int32_t        retStatus;

    FXNLOGD( "Playback Main Thread Start .." );

    retStatus    = EXIT_SUCCESS;
    p            = &gServerConfig.Playback;
    p->Main.bRun = true;

    while( true == p->Main.bRun )
    {
        FD_ZERO( &SocketSet );
        FD_SET( p->ServerSd, &SocketSet );

        Timeout.tv_sec  = 5;
        Timeout.tv_usec = 0;

        if( 0 < select( ( p->ServerSd + 1 ), &SocketSet, NULL, NULL, &Timeout ) )
        {
            if( false == p->Recv.bRun )
                continue;

            if( FD_ISSET( p->ServerSd, &SocketSet ) )
            {
                if( 0 <= p->ClientSd[ 0 ] )
                {
                    close( p->ClientSd[ 0] );
                    p->ClientSd[ 0 ] = -1;
                }

                retStatus = FmspServerAccept( p->ServerSd, &p->ClientSd[ 0 ], &Pid );

                if( EXIT_FAILURE == retStatus )
                {
                    FXNLOGD( "FmspServerAccept() error: %d", p->ClientSd[ 0 ] );
                    if( 0 <= p->ClientSd[ 0 ] )
                        close( p->ClientSd[ 0 ] );

                    p->ClientSd[ 0 ] = -1;
                    continue;
                }

                p->Recv.bSuspend = false;
                p->Recv.pFnWakup();
            }
        }
    }

    FXNLOGD( "Playback Main Thread Terminate .." );

    pthread_exit( NULL );
}

/* ----------------------------------------------------------- */
static void * FmspServerPlaybackRecvLoop( void * arg )
{
    FmspFramePlaybackCommand_u Command;
    FmspServer_t * p;
    int32_t        RecvLength;
    int32_t        TotalLength;
    int32_t        retStatus;
    int64_t        CurrentTs;
    int64_t        StartTime;
    int64_t        CurrentTime;

    FXNLOGD( "Playback Receive thread Start .." );

    p                = &gServerConfig.Playback;
    retStatus        = EXIT_SUCCESS;
    p->Recv.bRun     = true;
    p->Recv.bSuspend = true;

    while( true == p->Recv.bRun )
    {
        if( true == p->Recv.bSuspend )
            p->Recv.pFnSuspend();

        TotalLength = 0;
        ( void )memset( &Command, 0x00, sizeof( FmspFramePlaybackCommand_u ) );
        StartTime = LibUtilDateTimeGetUTCTimestamp();
        while( 0 <= p->ClientSd[ 0 ] )
        {
            RecvLength = recv( p->ClientSd[ 0 ], &Command.Buffer[ TotalLength ], sizeof( Command.Buffer ), 0 );

            if( 0 >= RecvLength )
            {
                RecvLength  = 0;
                TotalLength = 0;
                CurrentTime = LibUtilDateTimeGetUTCTimestamp();
                if( 1L < ( CurrentTime - StartTime ) )
                {
                    ( void )memset( &Command.Buffer[ 0 ], 0x00, sizeof( Command.Buffer ) );
                    if( ( 0 >= strlen( p->ClientName ) ) ||
                    	( LIB_UTILS_PROGRAM_NOT_EXECUTE == LibUtilProgramIsExecute( &p->ClientName[ 0 ] ) ) )
                    {
                    	( void )memset( &p->ClientName[ 0 ], 0x00, sizeof( p->ClientName ) );
                    	close( p->ClientSd[ 0 ] );
                    	p->ClientSd[ 0 ] = -1;
                    }
                    break;
                }
            }
            else
                TotalLength += RecvLength;

            if( sizeof( Command.Buffer ) <= TotalLength )
                break;
        }

        if( 0 > p->ClientSd[ 0 ] )
        {
            p->Recv.bSuspend = true;
            continue;
        }

        switch( Command.data.Type )
        {
            case FMSP_CMD_RESET:
                FXNLOGD( "Got Playback Streaming Reset Command .." );
                p->Send.bRenew = true;
                // if( false == p->Send.bSuspend )
                {
                    //p->Recv.bSuspend = true;
                    p->Send.bSuspend = true;
                    snprintf( &p->ClientName[ 0 ], FMSP_CMD_PROGRAM_NAME_SIZE, "%s", Command.data.Name );
                    FXNLOGD( "Config Playback Target Client Name:%s ", p->ClientName  );
                }
            break;

            case FMSP_CMD_CONFIG:
                if( false == p->Send.bSuspend )
                    retStatus = EXIT_FAILURE;
                else
                {
                    CurrentTs     = LibUtilHWTimerGetTimestamp();
                    p->TargetTs   = Command.data.TargetTs;
                    p->ArrivalTs  = Command.data.ArrivalTs;

                    FXNLOGD( "Config Playback Target Ts: %ld,  Arrvial Ts:%ld, Current Ts: %ld", p->TargetTs, p->ArrivalTs, CurrentTs );
                }
            break;

            case FMSP_CMD_START:
                if( false == p->Send.bSuspend )
                    retStatus = EXIT_FAILURE;
                else
                {
                    p->Send.bSuspend = false;
                    p->Send.pFnWakup();
                    FXNLOGD( "Got Playback Streaming Start to Sent Stream Command .." );
                }
            break;

            case FMSP_CMD_STOP:
                if( true == p->Send.bSuspend )
                    retStatus = EXIT_FAILURE;
                else
                {
                    FXNLOGD( "Got Playback Streaming Stop Sent Stream Command .." );
                    p->Recv.bSuspend = true;
                    p->Send.bSuspend = true;
                    FXNLOGD( "Close Client Socket .." );
                    close( p->ClientSd[ 0 ] );
                    p->ClientSd[ 0 ] = -1;
                }
            break;

            default:
                retStatus = EXIT_FAILURE;
            break;
        }
    }

    FXNLOGD( "Playback Receive thread Terminate ..\n" );

    pthread_exit( NULL );
}

/* ----------------------------------------------------------- */
static void * FmspServerPlaybackSendLoop( void * arg )
{
    FmspServerMediaConfig_t * pConfig;
    FmspServer_t            * p;
    FmspFramePlaybackInfo_u Package;
    int64_t TargetPTS;
    int64_t ArrivalPTS;
    int32_t retStatus;
    int32_t Codec;
    bool bStart;

    TargetPTS  = 0L;
    ArrivalPTS = 0L;

    p       = &gServerConfig.Playback;
    pConfig = &gServerConfig.Config;

    p->Send.bRun = true;
    bStart       = false;
    Codec        = pConfig->Codec;

    FXNLOGD( "Playback Sender thread Start .." );

    while( true == p->Send.bRun )
    {
        if( true == p->Send.bSuspend )
        {
            p->Send.pFnSuspend();

            p->Send.bRenew = true;
        }

        if (p->Send.bRenew == true) {
            p->Send.bRenew = false;
            ArrivalPTS = p->ArrivalTs;
            TargetPTS  = p->TargetTs;
            bStart     = false;
        }

        if( 0 > p->ClientSd[ 0 ] )
            p->Send.bSuspend = true;

        if( ( false == p->Send.bRun ) || ( true == p->Send.bSuspend ) )
            continue;

        ( void )memset( &Package, 0x00, sizeof( FmspFramePlaybackInfo_u ) );

        Package.data.IsRestart = FMSP_PACKAGE_MSG_NORMAL;
        if( true == pConfig->bChanged )
        {
            Codec = LibDBusClientCameraGetCodec();
            if( ( LIB_DBUS_CLIENT_CAMERA_CODEC_H264 != Codec ) &&
                ( LIB_DBUS_CLIENT_CAMERA_CODEC_H265 != Codec ) )
                Codec = LIB_DBUS_CLIENT_CAMERA_CODEC_H264;

            if( Codec != pConfig->Codec )
            {
                Package.data.IsRestart = FMSP_PACKAGE_MSG_RESTART;
                Package.data.IsStop    = FMSP_STREAMING_MEDIA_FINISH;
                Package.data.Type      = FMSP_PACKET_TYPE_VIDEO;
                pConfig->Codec         = Codec;

                if( ( 0 <= p->ClientSd[ 0 ] ) && ( false == p->Send.bSuspend ) )
                    send( p->ClientSd[ 0 ], &Package.Buffer[ 0 ], sizeof( FmspFramePlaybackInfo_u ), 0);

                continue;
            }

            pConfig->bChanged = false;
        }

        if( EXIT_SUCCESS == OryxStreamingGetFrame( &Package ) )
        {
            if( ( Package.data.pts >= TargetPTS ) &&
                ( FMSP_PACKET_TYPE_VIDEO == Package.data.Type ) )
                bStart = true;

            if( true == bStart )
            {
                if( ( ArrivalPTS <= Package.data.pts ) && ( 0 <= ArrivalPTS ) )
                    Package.data.IsStop = FMSP_STREAMING_MEDIA_FINISH;
                else
                    Package.data.IsStop = FMSP_STREAMING_MEDIA_NORMAL;

                if( ( 0 <= p->ClientSd[ 0 ] ) && ( false == p->Send.bSuspend ) )
                    send( p->ClientSd[ 0 ], &Package.Buffer[ 0 ], sizeof( FmspFramePlaybackInfo_u ), 0);

                if( FMSP_STREAMING_MEDIA_FINISH == Package.data.IsStop )
                    p->Send.bSuspend = true;
            }
        }
    }

    pConfig = NULL;
    p       = NULL;

    FXNLOGD( "Playback Sender thread Terminate .." );

    pthread_exit( NULL );
}

/*! ------------------------------------------------
 *    Liveview Server Callback
 *  ------------------------------------------------ */
static void * FmspServerLiveviewMainLoop( void * arg )
{
    FmspServer_t   * p;
    struct timeval Timeout;
    fd_set         SocketSet;
    pid_t          Pid;
    int32_t        Length;
    int32_t        retStatus;

    FXNLOGD( "Liveview Main Thread Start .." );

    retStatus    = EXIT_SUCCESS;
    p            = &gServerConfig.Liveview;
    p->Main.bRun = true;

    while( true == p->Main.bRun )
    {
        FD_ZERO( &SocketSet );
        FD_SET( p->ServerSd, &SocketSet );

        Timeout.tv_sec  = 5;
        Timeout.tv_usec = 0;

        if( 0 < select( ( p->ServerSd + 1 ), &SocketSet, NULL, NULL, &Timeout ) )
        {
            if( false == p->Recv.bRun )
                continue;

            if( FD_ISSET( p->ServerSd, &SocketSet ) )
            {
                if( 0 <= p->ClientSd[ 0 ] )
                {
                    close( p->ClientSd[ 0 ] );
                    p->ClientSd[ 0 ] = -1;
                }

                retStatus = FmspServerAccept( p->ServerSd, &p->ClientSd[ 0 ], &Pid );

                if( EXIT_FAILURE == retStatus )
                {
                    FXNLOGD( "FmspServerAccept() error: %d", p->ClientSd[ 0 ] );
                    if( 0 <= p->ClientSd[ 0 ] )
                        close( p->ClientSd[ 0 ] );

                    p->ClientSd[ 0 ] = -1;
                    continue;
                }

                p->Recv.bSuspend = false;
                p->Recv.pFnWakup();
            }
        }
    }

    FXNLOGD( "Liveview Main Thread Terminate .." );

    pthread_exit( NULL );
}

/* ----------------------------------------------------------- */
static void * FmspServerLiveviewRecvLoop( void * arg )
{
    FmspFramePlaybackCommand_u Command;
    FmspServer_t * p;
    int32_t        RecvLength;
    int32_t        TotalLength;
    int32_t        retStatus;
    int64_t        CurrentTs;
    int64_t        StartTime;
    int64_t        CurrentTime;

    FXNLOGD( "Liveview Receive thread Start .." );

    p                = &gServerConfig.Liveview;
    retStatus        = EXIT_SUCCESS;
    p->Recv.bRun     = true;
    p->Recv.bSuspend = true;

    while( true == p->Recv.bRun )
    {
        if( true == p->Recv.bSuspend )
            p->Recv.pFnSuspend();

        TotalLength = 0;
        ( void )memset( &Command, 0x00, sizeof( FmspFramePlaybackCommand_u ) );
        StartTime = LibUtilDateTimeGetUTCTimestamp();
        while( 0 <= p->ClientSd[ 0 ] )
        {
            RecvLength = recv( p->ClientSd[ 0 ], &Command.Buffer[ TotalLength ], sizeof( Command.Buffer ), 0 );

            if( 0 >= RecvLength )
            {
                RecvLength  = 0;
                TotalLength = 0;
                CurrentTime = LibUtilDateTimeGetUTCTimestamp();
                if( 1L < ( CurrentTime - StartTime ) )
                {
                    ( void )memset( &Command.Buffer[ 0 ], 0x00, sizeof( Command.Buffer ) );
                    if( ( 0 >= strlen( p->ClientName ) ) ||
                    	( LIB_UTILS_PROGRAM_NOT_EXECUTE == LibUtilProgramIsExecute( &p->ClientName[ 0 ] ) ) )
                    {
                    	( void )memset( &p->ClientName[ 0 ], 0x00, sizeof( p->ClientName ) );
                    	close( p->ClientSd[ 0 ] );
                    	p->ClientSd[ 0 ] = -1;
                    }
                    break;
                }
            }
            else
                TotalLength += RecvLength;

            if( sizeof( Command.Buffer ) <= TotalLength )
                break;
        }

        if( 0 > p->ClientSd[ 0 ] )
        {
            p->Recv.bSuspend = true;
            continue;
        }

        switch( Command.data.Type )
        {
            case FMSP_CMD_RESET:
                FXNLOGD( "Got Liveview Streaming Reset Command .." );
                if( false == p->Send.bSuspend )
                {
                    //p->Recv.bSuspend = true;
                    snprintf( &p->ClientName[ 0 ], FMSP_CMD_PROGRAM_NAME_SIZE, "%s", Command.data.Name );
                    FXNLOGD( "Config Liveview Target Client Name:%s ", p->ClientName  );
                    p->Send.bSuspend = true;
                }
            break;

            case FMSP_CMD_CONFIG:
                if( false == p->Send.bSuspend )
                    retStatus = EXIT_FAILURE;
                else
                {
                    CurrentTs     = LibUtilHWTimerGetTimestamp();
                    p->TargetTs   = Command.data.TargetTs;
                    p->ArrivalTs  = Command.data.ArrivalTs;

                    FXNLOGD( "Config Liveview Target Ts: %ld,  Arrvial Ts:%ld, Current Ts: %ld", p->TargetTs, p->ArrivalTs, CurrentTs );
                }
            break;

            case FMSP_CMD_START:
                if( false == p->Send.bSuspend )
                    retStatus = EXIT_FAILURE;
                else
                {
                    p->Send.bSuspend = false;
                    p->Send.pFnWakup();
                    FXNLOGD( "Got Liveview Streaming Start to Sent Stream Command .." );
                }
            break;

            case FMSP_CMD_STOP:
                if( true == p->Send.bSuspend )
                    retStatus = EXIT_FAILURE;
                else
                {
                    FXNLOGD( "Got Liveview Streaming Stop Sent Stream Command .." );
                    p->Recv.bSuspend = true;
                    p->Send.bSuspend = true;
                    FXNLOGD( "Close Client Socket .." );
                    close( p->ClientSd[ 0 ] );
                    p->ClientSd[ 0 ] = -1;
                }
            break;

            default:
                retStatus = EXIT_FAILURE;
            break;
        }
    }

    FXNLOGD( "Liveview Receive thread Terminate ..\n" );

    pthread_exit( NULL );
}

/* ----------------------------------------------------------- */
static void * FmspServerLiveviewSendLoop( void * arg )
{
    FmspServerMediaConfig_t * pConfig;
    FmspServer_t            * p;
    FmspFramePlaybackInfo_u Package;
    int64_t TargetPTS;
    int64_t ArrivalPTS;
    int32_t retStatus;
    int32_t Codec;
    bool bStart;

    TargetPTS  = 0L;
    ArrivalPTS = 0L;

    p       = &gServerConfig.Liveview;
    pConfig = &gServerConfig.Config;

    p->Send.bRun = true;
    bStart       = false;
    Codec        = pConfig->Codec;

    FXNLOGD( "Liveview Sender thread Start .." );

    while( true == p->Send.bRun )
    {
        if( true == p->Send.bSuspend )
        {
            p->Send.pFnSuspend();

            ArrivalPTS = p->ArrivalTs;
            TargetPTS  = p->TargetTs;
            bStart     = false;
        }

        if( 0 > p->ClientSd[ 0 ] )
            p->Send.bSuspend = true;

        if( ( false == p->Send.bRun ) || ( true == p->Send.bSuspend ) )
            continue;

        ( void )memset( &Package, 0x00, sizeof( FmspFramePlaybackInfo_u ) );

        Package.data.IsRestart = FMSP_PACKAGE_MSG_NORMAL;
        if( true == pConfig->bChanged )
        {
            Codec = LibDBusClientCameraGetCodec();
            if( ( LIB_DBUS_CLIENT_CAMERA_CODEC_H264 != Codec ) &&
                ( LIB_DBUS_CLIENT_CAMERA_CODEC_H265 != Codec ) )
                Codec = LIB_DBUS_CLIENT_CAMERA_CODEC_H264;

            if( Codec != pConfig->Codec )
            {
                Package.data.IsRestart = FMSP_PACKAGE_MSG_RESTART;
                Package.data.IsStop    = FMSP_STREAMING_MEDIA_FINISH;
                Package.data.Type      = FMSP_PACKET_TYPE_VIDEO;
                pConfig->Codec         = Codec;

                if( ( 0 <= p->ClientSd[ 0 ] ) && ( false == p->Send.bSuspend ) )
                    send( p->ClientSd[ 0 ], &Package.Buffer[ 0 ], sizeof( FmspFramePlaybackInfo_u ), 0);

                continue;
            }

            pConfig->bChanged = false;
        }

        if( EXIT_SUCCESS == OryxStreamingGetLiveFrame( &Package ) )
        {
            if( ( Package.data.pts >= TargetPTS ) &&
                ( FMSP_PACKET_TYPE_VIDEO == Package.data.Type ) )
                bStart = true;

            if( true == bStart )
            {
                if( ( ArrivalPTS <= Package.data.pts ) && ( 0 <= ArrivalPTS ) )
                    Package.data.IsStop = FMSP_STREAMING_MEDIA_FINISH;
                else
                    Package.data.IsStop = FMSP_STREAMING_MEDIA_NORMAL;

                if( ( 0 <= p->ClientSd[ 0 ] ) && ( false == p->Send.bSuspend ) )
                    send( p->ClientSd[ 0 ], &Package.Buffer[ 0 ], sizeof( FmspFramePlaybackInfo_u ), 0);

                if( FMSP_STREAMING_MEDIA_FINISH == Package.data.IsStop )
                    p->Send.bSuspend = true;
            }
        }
    }

    pConfig = NULL;
    p       = NULL;

    FXNLOGD( "Liveview Sender thread Terminate .." );

    pthread_exit( NULL );
}

/* ----------------------------------------------------------- */
static void FmspServerGetVideoConfiguration( void )
{
    FmspServerMediaConfig_t * p;

    p = &gServerConfig.Config;

    p->Framerate       = LibDBusClientCameraGetFrameRate();
    p->Codec           = LibDBusClientCameraGetCodec();
    p->ResolutionIndex = LibDBusClientCameraGetResolution();
    p->TargetBitrate   = LibDBusClientCameraGetBitRate();

    if( 0x00000000 != ( p->Framerate & LIB_DBUS_CLIENT_ERR_MASK ) )
        p->Framerate = LIB_DBUS_CLIENT_CAMERA_FRAMERATE_30;

    if( 0x00000000 != ( p->ResolutionIndex & LIB_DBUS_CLIENT_ERR_MASK ) )
        p->ResolutionIndex = LIB_DBUS_CLIENT_CAMERA_RESOLUTION_1440X1920;

    if( 0x00000000 != ( p->Codec & LIB_DBUS_CLIENT_ERR_MASK ) )
        p->Codec = LIB_DBUS_CLIENT_CAMERA_CODEC_H264;

    if( 0x00000000 != ( p->TargetBitrate & LIB_DBUS_CLIENT_ERR_MASK ) )
        p->TargetBitrate = LIB_DBUS_CLIENT_CAMERA_BITRATE_DEF;

    FXNLOGD( "Codec: %s", ( ( p->Codec == LIB_DBUS_CLIENT_CAMERA_CODEC_H264 )? "H.264" : "H.265" ) );
    FXNLOGD( "Resolution(%d): %d x %d", p->ResolutionIndex,
                                        p->TblResolution[ p->ResolutionIndex ].Width,
                                        p->TblResolution[ p->ResolutionIndex ].Height );
    FXNLOGD( "Frame rate: %d", p->Framerate );
    FXNLOGD( "Bitrate: %d", p->TargetBitrate );

    p->bChanged = false;
    p = NULL;
}

/* ------------------------------------------------------------------------- */

/* ---- Includes ----------------------------------------------------------- */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include "MLogger.h"
#include "am_base_include.h"
#include "am_define.h"
#include "am_export_if.h"
#include "OryxStreaming.h"

/* ---- Constants ---------------------------------------------------------- */
#define FMSP_LINK_INDEX_PLAYBACK    ( 0 )
#define FMSP_LINK_INDEX_LIVEVIEW    ( 1 )
#define FMSP_LINK_INDEX_MAX         ( 2 )

/* ---- Types -------------------------------------------------------------- */
typedef struct {
    AMExportPacket    Packet;
    AMIExportClient * Client;
    AMExportConfig    Config;
} FMSPSystemLink_t;

typedef struct {
    FMSPSystemLink_t Link[ FMSP_LINK_INDEX_MAX ];
} FmspConfig_t;

/* ---- Private Function Prototypes ---------------------------------------- */

/* ---- Variables ---------------------------------------------------------- */
static FmspConfig_t gConfig;
static FmspConfig_t gAVConfig;
static FmspConfig_t gAConfig;
static FmspConfig_t gVConfig;
static FmspConfig_t gVideoConfig;
static FmspConfig_t gAudioConfig;

/* ---- Global Functions --------------------------------------------------- */

/* ----------------------------------------------------------- */
int32_t OryxStreamingAudioCreate( void )
{
    FMSPSystemLink_t * p;
    int32_t Index = FMSP_LINK_INDEX_LIVEVIEW;

    std::string url = "/run/oryx/export.socket";
    int32_t  retStatus;
    printf( "[Media Provider][Audio] KvsStreamingAudioCreate: +\n");

    //for( Index = 0; Index < FMSP_LINK_INDEX_MAX; Index++ )
    //{
    p = &gAudioConfig.Link[ Index ];

    retStatus = EXIT_SUCCESS;

    p->Client           = nullptr;
    p->Config.video_map = 0x0; /*!< 0x01: Stream 1 / 0x02: Stream 2 ... */
    p->Config.audio_map = 1LL << 10; /*!< 48K / AAC */
    printf( "[Media Provider][Audio] KvsStreamingAudioCreate: 1\n");
    if( FMSP_LINK_INDEX_PLAYBACK == Index )
        p->Config.client_max_queue_size = 1024;
    else
        p->Config.client_max_queue_size = 0;
    //printf( "[Media Provider][Audio] KvsStreamingAudioCreate: 2\n");
    p->Client = AMIExportClient::create( &p->Config );
    //printf( "[Media Provider][Audio] KvsStreamingAudioCreate: 3\n");
    if( nullptr == p->Client )
    {
        printf( "[Media Provider][Audio] nullptr == p->Client\n");
        retStatus = EXIT_FAILURE;
        OryxStreamingDestroy();
    }
    else
    {
        printf( "[Media Provider][Audio] KvsStreamingAudioCreate: 2\n");
        retStatus = p->Client->connect_server( url.c_str() );
        if( 0 != retStatus )
        {
            printf( "[Media Provider][Audio] Connect media server failed, return code = %d\n", retStatus );
            retStatus = EXIT_FAILURE;
        }
    }
    //}

    printf( "[Media Provider][Audio] KvsStreamingAudioCreate: -\n");
    p = NULL;

    return retStatus;
}
int32_t OryxStreamingVideoCreate( void )
{
    FMSPSystemLink_t * p;
    int32_t Index = FMSP_LINK_INDEX_LIVEVIEW;

    std::string url = "/run/oryx/export.socket";
    int32_t  retStatus;
    //for( Index = 0; Index < FMSP_LINK_INDEX_MAX; Index++ )
    {
        Index = FMSP_LINK_INDEX_LIVEVIEW;
        p = &gVideoConfig.Link[ FMSP_LINK_INDEX_LIVEVIEW ];

        retStatus = EXIT_SUCCESS;

        p->Client           = nullptr;
        p->Config.video_map = 0x1; /*!< 0x01: Stream 1 / 0x02: Stream 2 ... */
        p->Config.audio_map = 0;
        if( FMSP_LINK_INDEX_PLAYBACK == Index )
            p->Config.client_max_queue_size = 1024;
        else
            p->Config.client_max_queue_size = 0;

        p->Client = AMIExportClient::create( &p->Config );

        if( nullptr == p->Client )
        {
            retStatus = EXIT_FAILURE;
            OryxStreamingDestroy();
        }
        else
        {
            retStatus = p->Client->connect_server( url.c_str() );
            if( 0 != retStatus )
            {
                printf( "[Media Provider] Connect media server failed, return code = %d\n", retStatus );
                retStatus = EXIT_FAILURE;
            }
        }
    }

    p = NULL;
    printf( "[Media Provider][Video] KvsStreamingVideoCreate, return code = %d\n", retStatus );
    return retStatus;
}
int32_t OryxStreamingCreate( void )
{
    FMSPSystemLink_t * p;
    int32_t Index;

    std::string url = "/run/oryx/export.socket";
    int32_t  retStatus;

    for( Index = 0; Index < FMSP_LINK_INDEX_MAX; Index++ )
    {
        p = &gConfig.Link[ Index ];

        retStatus = EXIT_SUCCESS;

        p->Client           = nullptr;
        p->Config.video_map = 0x1; /*!< 0x01: Stream 1 / 0x02: Stream 2 ... */
        p->Config.audio_map = 1LL << 10; /*!< 48K / AAC */
        if( FMSP_LINK_INDEX_PLAYBACK == Index )
            p->Config.client_max_queue_size = 1024;
        else
            p->Config.client_max_queue_size = 0;

        p->Client = AMIExportClient::create( &p->Config );

        if( nullptr == p->Client )
        {
            retStatus = EXIT_FAILURE;
            OryxStreamingDestroy();
        }
        else
        {
            retStatus = p->Client->connect_server( url.c_str() );
            if( 0 != retStatus )
            {
                printf( "[Media Provider] Connect media server failed, return code = %d\n", retStatus );
                retStatus = EXIT_FAILURE;
            }
        }
    }

    p = NULL;

    return retStatus;
}
int32_t OryxStreamingAVCreate( void )
{
    printf( "[Media Provider] OryxStreamingAVCreate: +\n");
    std::string url = "/run/oryx/export.socket";
    int32_t  retStatus;

    sAVconfig.video_map = 1; /*!< 0x01: Stream 1 / 0x02: Stream 2 ... */
    sAVconfig.audio_map = 1LL << 10; /*!< 48K / AAC */
    sAVconfig.client_max_queue_size = 0;
    printf( "[Media Provider] OryxStreamingAVCreate: 1\n");
    spAVClient = AMIExportClient::create(&sAVconfig);

    if( nullptr == spAVClient )
    {
        printf( "[Media Provider] OryxStreamingAVCreate: ERROR ERROR\n");
        retStatus = EXIT_FAILURE;
        OryxStreamingDestroy();
    }
    else
    {
        retStatus = spAVClient->connect_server( url.c_str() );
        if( 0 != retStatus )
        {
            printf( "[Media Provider] Connect media server failed, return code = %d\n", retStatus );
            retStatus = EXIT_FAILURE;
        } else {
            printf( "[Media Provider] OryxStreamingAVCreate: connect_server\n");
        }
    }

    printf( "[Media Provider][Video] OryxStreamingVideoCreate, return code = %d\n", retStatus );
    printf( "[Media Provider] OryxStreamingAVCreate: -\n");
    return retStatus;
}
int32_t OryxStreamingACreate( void )
{
    printf( "[Media Provider] OryxStreamingAVCreate: +\n");
    std::string url = "/run/oryx/export.socket";
    int32_t  retStatus;

    sAconfig.video_map = 0; /*!< 0x01: Stream 1 / 0x02: Stream 2 ... */
    sAconfig.audio_map = 1LL << 10; /*!< 48K / AAC */
    sAconfig.client_max_queue_size = 0;
    printf( "[Media Provider] OryxStreamingAVCreate: 1\n");
    spAClient = AMIExportClient::create(&sAconfig);

    if( nullptr == spAClient )
    {
        printf( "[Media Provider] OryxStreamingAVCreate: ERROR ERROR\n");
        retStatus = EXIT_FAILURE;
        OryxStreamingDestroy();
    }
    else
    {
        retStatus = spAClient->connect_server( url.c_str() );
        if( 0 != retStatus )
        {
            printf( "[Media Provider] Connect media server failed, return code = %d\n", retStatus );
            retStatus = EXIT_FAILURE;
        } else {
            printf( "[Media Provider] OryxStreamingAVCreate: connect_server\n");
        }
    }

    printf( "[Media Provider][Video] OryxStreamingVideoCreate, return code = %d\n", retStatus );
    printf( "[Media Provider] OryxStreamingAVCreate: -\n");
    return retStatus;
}

int32_t OryxStreamingVCreate( void )
{
    printf( "[Media Provider] OryxStreamingVCreate: +\n");
    std::string url = "/run/oryx/export.socket";
    int32_t  retStatus;
    sVconfig.video_map = 1; /*!< 0x01: Stream 1 / 0x02: Stream 2 ... */
    sVconfig.audio_map = 0;//1LL << 10; /*!< 48K / AAC */
    sVconfig.client_max_queue_size = 0;
    printf( "[Media Provider] OryxStreamingVCreate: 1\n");
    spVClient = AMIExportClient::create(&sVconfig);

    if( nullptr == spVClient )
    {
        printf( "[Media Provider] OryxStreamingVCreate: ERROR ERROR\n");
        retStatus = EXIT_FAILURE;
        OryxStreamingDestroy();
    }
    else
    {
        retStatus = spVClient->connect_server( url.c_str() );
        if( 0 != retStatus )
        {
            printf( "[Media Provider] Connect media server failed, return code = %d\n", retStatus );
            retStatus = EXIT_FAILURE;
        } else {
            printf( "[Media Provider] OryxStreamingAVCreate: connect_server\n");
        }
    }

    printf( "[Media Provider][Video] OryxStreamingVCreate, return code = %d\n", retStatus );
    printf( "[Media Provider] OryxStreamingVCreate: -\n");
    return retStatus;
}

/* ----------------------------------------------------------- */
void OryxStreamingDestroy( void )
{
    FMSPSystemLink_t * p;
    int32_t Index;

    for( Index = 0; Index < FMSP_LINK_INDEX_MAX; Index++ )
    {
        p = &gConfig.Link[ Index ];

        if( nullptr != p->Client )
        {
            p->Client->disconnect_server();
            p->Client->destroy();
            p->Client = nullptr;
        }
    }

    p = NULL;
}

int32_t OryxStreamingGetLiveVideoFrame( FmspFramePlaybackInfo_u * pInfo )
{
    FMSPSystemLink_t * p;
    int32_t          retStatus;
    p = &gVideoConfig.Link[ FMSP_LINK_INDEX_LIVEVIEW ];
    printf( "[Media Provider] +\n" );
    printf( "[Media Provider] OryxStreamingGetLiveVideoFrame: -> Client->receive\n" );
    retStatus = p->Client->receive( &p->Packet, -1 );
    printf( "[Media Provider] OryxStreamingGetLiveVideoFrame: <- Client->receive\n" );
    if( AM_RESULT_ERR_SERVER_DOWN == retStatus )
        printf( "[Media Provider] Disconnect media server\n" );
    else if( 0 != retStatus )
        printf( "[Media Provider] Receive media frame error\n" );
    else
    {
        retStatus = 0;
        if( AM_EXPORT_PACKET_TYPE_VIDEO_DATA == p->Packet.packet_type )
        {
            if( ( AM_VIDEO_FRAME_TYPE_IDR == p->Packet.frame_type ) ||
                ( AM_VIDEO_FRAME_TYPE_I == p->Packet.frame_type ) ) {
                printf( "[Media Provider] OryxStreamingGetLiveVideoFrame: 1-1\n" );
                pInfo->data.IsKeyFrame = true;
            } else {
                //printf( "[Media Provider] OryxStreamingGetLiveVideoFrame: 1-2\n" );
                pInfo->data.IsKeyFrame = false;
            }
            pInfo->data.Type = FMSP_PACKET_TYPE_VIDEO;
            pInfo->data.pts  = p->Packet.pts;
            pInfo->data.Size = p->Packet.data_size;
            ( void )memcpy( &pInfo->data.Buffer[ 0 ], p->Packet.data_ptr, p->Packet.data_size );
        }
        else
        {
            printf( "[Media Provider] Others\n" );
            pInfo->data.Type = FMSP_PACKET_TYPE_OTHER;
            retStatus = 0;
        }
        printf( "[Media Provider] OryxStreamingGetLiveVideoFrame: -> Client->release\n" );
        p->Client->release( &p->Packet );
        printf( "[Media Provider] OryxStreamingGetLiveVideoFrame: <- Client->release\n" );
    }
    printf( "[Media Provider] -\n" );
    p = NULL;

    return ( ( 0 == retStatus ) ? EXIT_SUCCESS : EXIT_FAILURE );
}
int32_t OryxStreamingGetLiveAudioFrame( FmspFramePlaybackInfo_u * pInfo )
{
    FMSPSystemLink_t * p;
    int32_t          retStatus;

    p = &gAudioConfig.Link[ FMSP_LINK_INDEX_LIVEVIEW ];
    printf( "[Media Provider] OryxStreamingGetLiveAudioFrame: -> client->receive\n" );
    retStatus = p->Client->receive( &p->Packet, -1 );
    printf( "[Media Provider] OryxStreamingGetLiveAudioFrame: <- client->receive\n" );
    if( AM_RESULT_ERR_SERVER_DOWN == retStatus )
        printf( "[Media Provider] Disconnect media server\n" );
    else if( 0 != retStatus )
        printf( "[Media Provider] Receive media frame error\n" );
    else
    {
        retStatus = 0;
        if( AM_EXPORT_PACKET_TYPE_AUDIO_DATA == p->Packet.packet_type )
        {
            pInfo->data.IsKeyFrame = false;
            pInfo->data.Type = FMSP_PACKET_TYPE_AUDIO;
            pInfo->data.pts  = p->Packet.pts;
            pInfo->data.Size = p->Packet.data_size - 7;
            ( void )memcpy( &pInfo->data.Buffer[ 0 ], ( p->Packet.data_ptr + 7 ), ( p->Packet.data_size - 7 ) );
        }
        else
        {
            printf( "[Media Provider] Others\n" );
            pInfo->data.Type = FMSP_PACKET_TYPE_OTHER;
            retStatus = 0;
        }

        p->Client->release( &p->Packet );
    }

    p = NULL;

    return ( ( 0 == retStatus ) ? EXIT_SUCCESS : EXIT_FAILURE );
}
int32_t OryxStreamingGetFrame( FmspFramePlaybackInfo_u * pInfo )
{
    FMSPSystemLink_t * p;
    int32_t          retStatus;

    p = &gConfig.Link[ FMSP_LINK_INDEX_PLAYBACK ];

    retStatus = p->Client->receive( &p->Packet, -1 );
    if (AM_RESULT_ERR_SERVER_DOWN == retStatus) {
        printf("[Media Provider] Disconnect media server\n");
    } else if( 0 != retStatus ) {
            printf("[Media Provider] Receive media frame error\n");
    } else {
        retStatus = 0;
        if( AM_EXPORT_PACKET_TYPE_VIDEO_DATA == p->Packet.packet_type )
        {
            if( ( AM_VIDEO_FRAME_TYPE_IDR == p->Packet.frame_type ) ||
                ( AM_VIDEO_FRAME_TYPE_I == p->Packet.frame_type ) )
                pInfo->data.IsKeyFrame = true;
            else
                pInfo->data.IsKeyFrame = false;

            pInfo->data.Type = FMSP_PACKET_TYPE_VIDEO;
            pInfo->data.pts  = p->Packet.pts;
            pInfo->data.Size = p->Packet.data_size;
            ( void )memcpy( &pInfo->data.Buffer[ 0 ], p->Packet.data_ptr, p->Packet.data_size );

        }
        else if( AM_EXPORT_PACKET_TYPE_AUDIO_DATA == p->Packet.packet_type )
        {
            pInfo->data.IsKeyFrame = false;
            pInfo->data.Type = FMSP_PACKET_TYPE_AUDIO;
            pInfo->data.pts  = p->Packet.pts;
            pInfo->data.Size = p->Packet.data_size - 7;
            ( void )memcpy( &pInfo->data.Buffer[ 0 ], ( p->Packet.data_ptr + 7 ), ( p->Packet.data_size - 7 ) );
        }
        else
        {
            retStatus = -1;
        }

        p->Client->release( &p->Packet );
    }

    p = NULL;

    return ( ( 0 == retStatus ) ? EXIT_SUCCESS : EXIT_FAILURE );
}
int32_t OryxStreamingGetLiveFrame( FmspFramePlaybackInfo_u * pInfo )
{
    FMSPSystemLink_t * p;
    int32_t          retStatus;

    p = &gConfig.Link[ FMSP_LINK_INDEX_LIVEVIEW ];

    retStatus = p->Client->receive( &p->Packet, -1 );
    if( AM_RESULT_ERR_SERVER_DOWN == retStatus )
        printf( "[Media Provider] Disconnect media server\n" );
    else if( 0 != retStatus )
        printf( "[Media Provider] Receive media frame error\n" );
    else
    {
        retStatus = 0;
        if( AM_EXPORT_PACKET_TYPE_VIDEO_DATA == p->Packet.packet_type )
        {
            if( ( AM_VIDEO_FRAME_TYPE_IDR == p->Packet.frame_type ) ||
                ( AM_VIDEO_FRAME_TYPE_I == p->Packet.frame_type ) )
                pInfo->data.IsKeyFrame = true;
            else
                pInfo->data.IsKeyFrame = false;

            pInfo->data.Type = FMSP_PACKET_TYPE_VIDEO;
            pInfo->data.pts  = p->Packet.pts;
            pInfo->data.Size = p->Packet.data_size;
            ( void )memcpy( &pInfo->data.Buffer[ 0 ], p->Packet.data_ptr, p->Packet.data_size );

        }
        else if( AM_EXPORT_PACKET_TYPE_AUDIO_DATA == p->Packet.packet_type )
        {
            pInfo->data.IsKeyFrame = false;
            pInfo->data.Type = FMSP_PACKET_TYPE_AUDIO;
            pInfo->data.pts  = p->Packet.pts;
            pInfo->data.Size = p->Packet.data_size - 7;
            ( void )memcpy( &pInfo->data.Buffer[ 0 ], ( p->Packet.data_ptr + 7 ), ( p->Packet.data_size - 7 ) );
        }
        else
        {
            printf( "[Media Provider] Others\n" );
            pInfo->data.Type = FMSP_PACKET_TYPE_OTHER;
            retStatus = 0;
        }

        p->Client->release( &p->Packet );
    }

    p = NULL;

    return ( ( 0 == retStatus ) ? EXIT_SUCCESS : EXIT_FAILURE );
}

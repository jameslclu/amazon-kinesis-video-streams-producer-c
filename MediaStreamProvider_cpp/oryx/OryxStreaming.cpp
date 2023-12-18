/* ---- Includes ----------------------------------------------------------- */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "am_base_include.h"
#include "am_define.h"
#include "am_export_if.h"
//#include "gst/gst.h"
//#include "gst/app/gstappsink.h"

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

/* ---- Global Functions --------------------------------------------------- */

/* ----------------------------------------------------------- */
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

/* ----------------------------------------------------------- */
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

/* ----------------------------------------------------------- */
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

/* ---- Private Function --------------------------------------------------- */

/* ---- Call-back Functions ------------------------------------------------ */

/* ------------------------------------------------------------------------- */

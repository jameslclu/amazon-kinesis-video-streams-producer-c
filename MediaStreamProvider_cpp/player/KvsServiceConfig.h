//
// Created by jh on 11/8/23.
//

#ifndef KVSPRODUCERSERVICE_KVSSERVICECONFIG_H
#define KVSPRODUCERSERVICE_KVSSERVICECONFIG_H
#define DEFAULT_RETENTION_PERIOD        2 * HUNDREDS_OF_NANOS_IN_AN_HOUR
#define DEFAULT_BUFFER_DURATION         120 * HUNDREDS_OF_NANOS_IN_A_SECOND
#define DEFAULT_CALLBACK_CHAIN_COUNT    5
#define DEFAULT_KEY_FRAME_INTERVAL      45
#define DEFAULT_FPS_VALUE               25
#define DEFAULT_STREAM_DURATION         20 * HUNDREDS_OF_NANOS_IN_A_SECOND
#define SAMPLE_AUDIO_FRAME_DURATION     (20 * HUNDREDS_OF_NANOS_IN_A_MILLISECOND)
#define SAMPLE_VIDEO_FRAME_DURATION     (HUNDREDS_OF_NANOS_IN_A_SECOND / DEFAULT_FPS_VALUE)
#define AAC_AUDIO_TRACK_SAMPLING_RATE   48000
#define ALAW_AUDIO_TRACK_SAMPLING_RATE  8000
#define AAC_AUDIO_TRACK_CHANNEL_CONFIG  2
#define ALAW_AUDIO_TRACK_CHANNEL_CONFIG 1
#define AUDIO_CODEC_NAME_MAX_LENGTH     5
#define VIDEO_CODEC_NAME_MAX_LENGTH     5
#define AUDIO_CODEC_NAME_ALAW           "alaw"
#define AUDIO_CODEC_NAME_AAC            "aac"
#define VIDEO_CODEC_NAME_H264           "h264"

#define NUMBER_OF_VIDEO_FRAME_FILES 403
#define NUMBER_OF_AUDIO_FRAME_FILES 582
#define NUMBER_OF_VIDEO_FRAME 403
#define NUMBER_OF_AUDIO_FRAME 582


#define FILE_LOGGING_BUFFER_SIZE (100 * 1024)
#define MAX_NUMBER_OF_LOG_FILES  5

//#define USER_DB_DIR "/data/etc/fxnserviceb"
//#define BUS_NAME "fxn.kvsservice"
//#define OBJECT_PATH "/fxn/kvsservice"
//#define INTERFACE_NAME "fxn.kvsservice"

#endif // KVSPRODUCERSERVICE_KVSSERVICECONFIG_H

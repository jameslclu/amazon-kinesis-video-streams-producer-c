//
// Created by jh on 12/18/23.
//

#ifndef KVSAUDIOVIDEOSTREAMINGSAMPLE_ALLPCSTRUCTURES_H
#define KVSAUDIOVIDEOSTREAMINGSAMPLE_ALLPCSTRUCTURES_H
#include <com/amazonaws/kinesis/video/cproducer/Include.h>

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
#define VIDEO_CODEC_NAME_H265           "h265"

#define NUMBER_OF_VIDEO_FRAME_FILES 403
#define NUMBER_OF_AUDIO_FRAME_FILES 582

typedef struct {
    PBYTE buffer;
    UINT32 size;
} FrameData, *PFrameData;

typedef struct {
    volatile ATOMIC_BOOL firstVideoFramePut;
    UINT64 streamStopTime;
    UINT64 streamStartTime;
    STREAM_HANDLE streamHandle;
    CHAR sampleDir[MAX_PATH_LEN + 1];
    FrameData audioFrames[NUMBER_OF_AUDIO_FRAME_FILES];
    FrameData videoFrames[NUMBER_OF_VIDEO_FRAME_FILES];
    BOOL firstFrame;
    UINT64 startTime;
} PCSampleCustomData, *PPCSampleCustomData;

#endif // KVSAUDIOVIDEOSTREAMINGSAMPLE_ALLPCSTRUCTURES_H

#ifndef KINESISVIDEOPRODUCERC_STREAMSOURCE_H
#define KINESISVIDEOPRODUCERC_STREAMSOURCE_H
#include "com/amazonaws/kinesis/video/common/CommonDefs.h"
#include <com/amazonaws/kinesis/video/cproducer/Include.h>

#include "KvsServiceConfig.h"
#include "AllInterfaces.h"

typedef struct {
    PBYTE buffer;
    UINT32 size;
} FrameData, *PFrame_Data;

//typedef struct {
//    volatile ATOMIC_BOOL firstVideoFramePut;
//    UINT64 streamStopTime;
//    UINT64 streamStartTime;
//    STREAM_HANDLE streamHandle;
//    CHAR sampleDir[MAX_PATH_LEN + 1];
//    FrameData audioFrames[NUMBER_OF_AUDIO_FRAME_FILES];
//    FrameData videoFrames[NUMBER_OF_VIDEO_FRAME_FILES];
//    BOOL firstFrame;
//    UINT64 startTime;
//} SampleCustomData, *PSampleCustomData;

//static PSampleCustomData gStreamSource;
//static SampleCustomData data;

class SampleStreamSource : public IStreamSource {
  public:
    volatile ATOMIC_BOOL firstVideoFramePut;
    //UINT64 startTime;
    //UINT64 streamStartTime;
    //UINT64 streamStopTime;
    //BOOL firstFrame;
    CHAR sampleDir[MAX_PATH_LEN + 1];
    STREAM_HANDLE streamHandle;
    FrameData audioFrames[NUMBER_OF_AUDIO_FRAME];
    FrameData videoFrames[NUMBER_OF_VIDEO_FRAME];

    int Init() {
        CHAR audioCodec[AUDIO_CODEC_NAME_MAX_LENGTH] = {0};
        CHAR videoCodec[VIDEO_CODEC_NAME_MAX_LENGTH] = {0};

        //MEMSET(&data, 0x00, SIZEOF(SampleCustomData));
        //gStreamSource = &data;
        STRNCPY(audioCodec, AUDIO_CODEC_NAME_AAC, STRLEN(AUDIO_CODEC_NAME_AAC));   // aac audio by default
        STRNCPY(videoCodec, VIDEO_CODEC_NAME_H264, STRLEN(VIDEO_CODEC_NAME_H264)); // h264 video by default
        CHAR filePath[MAX_PATH_LEN + 1];
        UINT64 fileSize = 0;

        // Load Audio
        int i = 0;
        for (i = 0; i < NUMBER_OF_AUDIO_FRAME_FILES; ++i) {
            SNPRINTF(filePath, MAX_PATH_LEN, "%s/%sSampleFrames/sample-%03d.%s", this->sampleDir, audioCodec, i + 1, audioCodec);
            readFile(filePath, TRUE, NULL, &fileSize);
            this->audioFrames[i].buffer = (PBYTE) MEMALLOC(fileSize);
            this->audioFrames[i].size = fileSize;
            readFile(filePath, TRUE, this->audioFrames[i].buffer, &fileSize);
        }

        // Load Video
        for (i = 0; i < NUMBER_OF_VIDEO_FRAME_FILES; ++i) {
            SNPRINTF(filePath, MAX_PATH_LEN, "%s/%sSampleFrames/frame-%03d.%s", this->sampleDir, videoCodec, i + 1, videoCodec);
            readFile(filePath, TRUE, NULL, &fileSize);
            this->videoFrames[i].buffer = (PBYTE) MEMALLOC(fileSize);
            this->videoFrames[i].size = fileSize;
            readFile(filePath, TRUE, this->videoFrames[i].buffer, &fileSize);
        }
        return 0;
    }

    int Init_Time( BOOL firstFrame,
                               UINT64 startTime,
                               UINT64 streamStopTime,
                               UINT64 streamStartTime) {
        //this->firstFrame = firstFrame;
        //this->startTime = startTime;
        //this->streamStartTime = streamStartTime;
        //this->streamStopTime = streamStopTime;
        return 0;
    }

    int SetDataSource(PCHAR _path) {
        STRCPY(this->sampleDir, (PCHAR) _path);
        return 0;
    }

    int Reset() {
        mAudioIndex = 0;
        mVideoIndex = 0;
        return 0;
    }

    int Deinit() {
        for (int i = 0; i < NUMBER_OF_AUDIO_FRAME_FILES; ++i) {
            SAFE_MEMFREE(audioFrames[i].buffer);
        }

        for (int i = 0; i < NUMBER_OF_VIDEO_FRAME_FILES; ++i) {
            SAFE_MEMFREE(videoFrames[i].buffer);
        }
        return 0;
    }

    int GetStatus() { return 0; }
    int SetHandler(STREAM_HANDLE handler) {
        this->streamHandle = handler;
        return 0;
    }
    int mVideoIndex;
    int mAudioIndex;
    int GetVideoFrame(PBYTE* pdata, UINT32 *psize, UINT64* pPTS) {
        if (mVideoIndex >= NUMBER_OF_VIDEO_FRAME_FILES) return 1;
        *pdata = this->videoFrames[mVideoIndex].buffer;
        *psize = this->videoFrames[mVideoIndex].size;
        *pPTS = mVideoIndex / DEFAULT_TIME_UNIT_IN_NANOS;
        mVideoIndex++;
        return 0;
    }

    int GetAudioFrame(PBYTE* pdata, UINT32 *psize, UINT64* pPTS) {
        if (this->mAudioIndex >= NUMBER_OF_AUDIO_FRAME_FILES) return 1;
        *pdata = this->audioFrames[this->mAudioIndex].buffer;
        *psize = this->audioFrames[this->mAudioIndex].size;
        *pPTS = this->mAudioIndex / DEFAULT_TIME_UNIT_IN_NANOS;
        this->mAudioIndex++;
        return 0;
    }
};
#endif // KINESISVIDEOPRODUCERC_STREAMSOURCE_H

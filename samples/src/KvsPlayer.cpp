//
// Created by jh on 11/9/23.
//
#include "KvsPlayer.h"
#include "ComponentProvider.h"
#include "KvsServiceConfig.h"
static UINT64 gStartTime;
static UINT64 gStreamStartTime;
static UINT64 gStreamStopTime;
static volatile ATOMIC_BOOL gIsFirstVideoFramePut = FALSE;
static PSEMAPHORE_HANDLE mpStreamHandle;
static UINT8 gEventsEnabled = 0;

static PVOID putVideoFrameRoutine(PVOID args)
{
    STATUS retStatus = STATUS_SUCCESS;
    Frame frame;
    UINT32 fileIndex = 0;
    STATUS status;
    UINT64 runningTime;
    DOUBLE startUpLatency;

    ComponentProvider::GetInstance()->GetStreamSource(FAKE)
        ->GetVideoFrame(fileIndex, &frame.frameData, &frame.size);

    frame.version = FRAME_CURRENT_VERSION;
    frame.trackId = DEFAULT_VIDEO_TRACK_ID;
    frame.duration = 0;
    frame.decodingTs = 0;
    frame.presentationTs = 0;
    frame.index = 0;

    // video track is used to mark new fragment. A new fragment is generated for every frame with FRAME_FLAG_KEY_FRAME
    frame.flags = fileIndex % DEFAULT_KEY_FRAME_INTERVAL == 0 ? FRAME_FLAG_KEY_FRAME : FRAME_FLAG_NONE;

    while (GETTIME() < gStreamStopTime) {
        //status = putKinesisVideoFrame(*mpStreamHandle, &frame);
        status = ComponentProvider::GetInstance()->GetKvsRender(AWSPRODUCKER)
                     ->PutVideoFrame(NULL, &frame);
        if (gIsFirstVideoFramePut) {
            startUpLatency = (DOUBLE) (GETTIME() - gStartTime) / (DOUBLE) HUNDREDS_OF_NANOS_IN_A_MILLISECOND;
            DLOGD("Start up latency: %lf ms", startUpLatency);
            //psStreamSource->firstFrame = FALSE;
            gIsFirstVideoFramePut = FALSE;
        } else if (frame.flags == FRAME_FLAG_KEY_FRAME && gEventsEnabled) {
            // generate an image and notification event at the start of the video stream.
            //putKinesisVideoEventMetadata(data->streamHandle, STREAM_EVENT_TYPE_NOTIFICATION | STREAM_EVENT_TYPE_IMAGE_GENERATION, NULL);
            putKinesisVideoEventMetadata(*mpStreamHandle, STREAM_EVENT_TYPE_NOTIFICATION | STREAM_EVENT_TYPE_IMAGE_GENERATION, NULL);
            // only push this once in this sample. A customer may use this whenever it is necessary though
            gEventsEnabled = 0;
        }

        ATOMIC_STORE_BOOL(&gIsFirstVideoFramePut, TRUE);

        if (STATUS_FAILED(status)) {
            printf("putKinesisVideoFrame failed with 0x%08x\n", status);
            status = STATUS_SUCCESS;
        }

        frame.presentationTs += SAMPLE_VIDEO_FRAME_DURATION;
        frame.decodingTs = frame.presentationTs;
        frame.index++;

        fileIndex = (fileIndex + 1) % NUMBER_OF_VIDEO_FRAME_FILES;
        frame.flags = fileIndex % DEFAULT_KEY_FRAME_INTERVAL == 0 ? FRAME_FLAG_KEY_FRAME : FRAME_FLAG_NONE;
        ComponentProvider::GetInstance()->GetStreamSource(FAKE)->GetVideoFrame(fileIndex, &frame.frameData, &frame.size);

        // synchronize putKinesisVideoFrame to running time
        runningTime = GETTIME() - gStreamStartTime;//psStreamSource->streamStartTime;
        if (runningTime < frame.presentationTs) {
            // reduce sleep time a little for smoother video
            THREAD_SLEEP((frame.presentationTs - runningTime) * 0.9);
        }
    }

CleanUp:

    if (retStatus != STATUS_SUCCESS) {
        printf("putVideoFrameRoutine failed with 0x%08x", retStatus);
    }

    return (PVOID) (ULONG_PTR) retStatus;
}

static PVOID putAudioFrameRoutine(PVOID args)
{
    STATUS retStatus = STATUS_SUCCESS;
    Frame frame;
    UINT32 fileIndex = 0;
    STATUS status;
    UINT64 runningTime;

    ComponentProvider::GetInstance()->GetStreamSource(FAKE)
        ->GetAudioFrame(fileIndex, &frame.frameData, &frame.size);

    frame.version = FRAME_CURRENT_VERSION;
    frame.trackId = DEFAULT_AUDIO_TRACK_ID;
    frame.duration = 0;
    frame.decodingTs = 0;     // relative time mode
    frame.presentationTs = 0; // relative time mode
    frame.index = 0;
    frame.flags = FRAME_FLAG_NONE; // audio track is not used to cut fragment

    while (GETTIME() < gStreamStopTime) {
        // no audio can be put until first video frame is put
        if (ATOMIC_LOAD_BOOL(&gIsFirstVideoFramePut)) {
            status = ComponentProvider::GetInstance()->GetKvsRender(AWSPRODUCKER)
                ->PutVideoFrame(NULL, &frame);
            //status = putKinesisVideoFrame(*mpStreamHandle, &frame);
            if (STATUS_FAILED(status)) {
                printf("putKinesisVideoFrame for audio failed with 0x%08x\n", status);
                status = STATUS_SUCCESS;
            }

            frame.presentationTs += SAMPLE_AUDIO_FRAME_DURATION;
            frame.decodingTs = frame.presentationTs;
            frame.index++;

            fileIndex = (fileIndex + 1) % NUMBER_OF_AUDIO_FRAME_FILES;
            ComponentProvider::GetInstance()->GetStreamSource(FAKE)->GetAudioFrame(fileIndex, &frame.frameData, &frame.size);

            // synchronize putKinesisVideoFrame to running time
            runningTime = GETTIME() - gStreamStartTime;
            if (runningTime < frame.presentationTs) {
                THREAD_SLEEP(frame.presentationTs - runningTime);
            }
        }
    }

CleanUp:
    if (retStatus != STATUS_SUCCESS) {
        printf("putAudioFrameRoutine failed with 0x%08x", retStatus);
    }

    return (PVOID) (ULONG_PTR) retStatus;
}
KvsPlayer::KvsPlayer() {

}

KvsPlayer::~KvsPlayer() {

}

int KvsPlayer::Init() {

    return 0;
}

int KvsPlayer::Deinit()
{
    return 0;
}

// Get the data source from the ResourceProvider
// int SetSource(IStreamSource &streamSource);
// Get the render from the RenderPROVIDER
// int SetRender(KvsOutput &kvsOutput);
static TID audioSendTid, videoSendTid;
int KvsPlayer::Start() {
    gStreamStartTime = GETTIME();
    gStartTime = gStreamStartTime;
    gStreamStopTime = gStreamStartTime + DEFAULT_STREAM_DURATION;
    gIsFirstVideoFramePut = FALSE;

    THREAD_CREATE(&videoSendTid, putVideoFrameRoutine, NULL);
    THREAD_CREATE(&audioSendTid, putAudioFrameRoutine, NULL);

    THREAD_JOIN(videoSendTid, nullptr);
    THREAD_JOIN(audioSendTid, nullptr);
    return 0;
}

int KvsPlayer::Pause() {
    return 0;
}

int KvsPlayer::Stop() {
    return 0;
}
int GetStatus(KvsPlayerState state);
#include "KvsPlayer.h"

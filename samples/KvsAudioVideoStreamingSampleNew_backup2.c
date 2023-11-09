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

#define NUMBER_OF_VIDEO_FRAME_FILES 403
#define NUMBER_OF_AUDIO_FRAME_FILES 582

#define FILE_LOGGING_BUFFER_SIZE (100 * 1024)
#define MAX_NUMBER_OF_LOG_FILES  5
STREAM_HANDLE mStreamHandle = INVALID_STREAM_HANDLE_VALUE;
PSEMAPHORE_HANDLE mpStreamHandle = &mStreamHandle;
UINT8 gEventsEnabled = 0;

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
} SampleCustomData, *PSampleCustomData;

PSampleCustomData gStreamSource;
//==============================================================================
//                              StreamSource
//==============================================================================
int StreamSource_SetHandler(STREAM_HANDLE handler) {
    gStreamSource->streamHandle = handler;
}
int StreamSource_Init() {
    CHAR audioCodec[AUDIO_CODEC_NAME_MAX_LENGTH] = {0};
    CHAR videoCodec[VIDEO_CODEC_NAME_MAX_LENGTH] = {0};

    STRNCPY(audioCodec, AUDIO_CODEC_NAME_AAC, STRLEN(AUDIO_CODEC_NAME_AAC));   // aac audio by default
    STRNCPY(videoCodec, VIDEO_CODEC_NAME_H264, STRLEN(VIDEO_CODEC_NAME_H264)); // h264 video by default
    CHAR filePath[MAX_PATH_LEN + 1];
    UINT64 fileSize = 0;

    // Load Audio
    int i = 0;
    for (i = 0; i < NUMBER_OF_AUDIO_FRAME_FILES; ++i) {
        SNPRINTF(filePath, MAX_PATH_LEN, "%s/%sSampleFrames/sample-%03d.%s", gStreamSource->sampleDir, audioCodec, i + 1, audioCodec);
        readFile(filePath, TRUE, NULL, &fileSize);
        gStreamSource->audioFrames[i].buffer = (PBYTE) MEMALLOC(fileSize);
        gStreamSource->audioFrames[i].size = fileSize;
        readFile(filePath, TRUE, gStreamSource->audioFrames[i].buffer, &fileSize);
    }

    // Load Video
    for (i = 0; i < NUMBER_OF_VIDEO_FRAME_FILES; ++i) {
        SNPRINTF(filePath, MAX_PATH_LEN, "%s/%sSampleFrames/frame-%03d.%s", gStreamSource->sampleDir, videoCodec, i + 1, videoCodec);
        readFile(filePath, TRUE, NULL, &fileSize);
        gStreamSource->videoFrames[i].buffer = (PBYTE) MEMALLOC(fileSize);
        gStreamSource->videoFrames[i].size = fileSize;
        readFile(filePath, TRUE, gStreamSource->videoFrames[i].buffer, &fileSize);
    }

    return 0;
}

int StreamSource_Init_Time( BOOL firstFrame,
                            UINT64 startTime,
                            UINT64 streamStopTime,
                            UINT64 streamStartTime) {
    gStreamSource->firstFrame = firstFrame;
    gStreamSource->startTime = startTime;
    gStreamSource->streamStartTime = streamStartTime;
    gStreamSource->streamStopTime = streamStopTime;
    return 0;
}

int StreamSource_SetDataSource(PCHAR _path) {
    STRCPY(gStreamSource->sampleDir, (PCHAR) _path);
    return 0;
}

int StreamSource_Deinit() {
    return 0;
}

// return size;
int StreamSource_GetVideoFrame(int index, PBYTE* pdata, UINT32 *psize) {
    *pdata = gStreamSource->videoFrames[index].buffer;
    *psize = gStreamSource->videoFrames[index].size;
    return 0;
}

int StreamSource_GetAudioFrame(int index, PBYTE* pdata, UINT32 *psize) {
    *pdata = gStreamSource->audioFrames[index].buffer;
    *psize = gStreamSource->audioFrames[index].size;
    return 0;
}

//==============================================================================
//                              KVSProducer
//==============================================================================

PCHAR mStreamName;
PDeviceInfo mpDeviceInfo;
PStreamInfo mpStreamInfo = NULL;
PTrackInfo mpAudioTrack = NULL;
BYTE mAACAudioCpd[KVS_AAC_CPD_SIZE_BYTE];
PClientCallbacks mpClientCallbacks = NULL;
PStreamCallbacks mpStreamCallbacks = NULL;
CLIENT_HANDLE mClientHandle = INVALID_CLIENT_HANDLE_VALUE;
STREAM_HANDLE mKVSStreamHandle;

int KVSProducer_SetStreamName(PCHAR name) {
    mStreamName = name;
    return 0;
}

int KVSProducer_SetStreamHandler(STREAM_HANDLE handle) {

    return 0;
}


int KVSProducer_Init() {
    createDefaultDeviceInfo(&mpDeviceInfo);
    mpDeviceInfo->clientInfo.loggerLogLevel = LOG_LEVEL_DEBUG;
    //Init-001
    {
        createRealtimeAudioVideoStreamInfoProviderWithCodecs(mStreamName, DEFAULT_RETENTION_PERIOD, DEFAULT_BUFFER_DURATION, VIDEO_CODEC_ID_H264,
                                                                        AUDIO_CODEC_ID_AAC, &mpStreamInfo);
        // set up audio cpd.
        mpAudioTrack = mpStreamInfo->streamCaps.trackInfoList[0].trackId == DEFAULT_AUDIO_TRACK_ID ? &mpStreamInfo->streamCaps.trackInfoList[0]
                                                                                                 : &mpStreamInfo->streamCaps.trackInfoList[1];
        mpAudioTrack->codecPrivateData = mAACAudioCpd;
        mpAudioTrack->codecPrivateDataSize = KVS_AAC_CPD_SIZE_BYTE;
        mkvgenGenerateAacCpd(AAC_LC, AAC_AUDIO_TRACK_SAMPLING_RATE, AAC_AUDIO_TRACK_CHANNEL_CONFIG, mpAudioTrack->codecPrivateData,
                             mpAudioTrack->codecPrivateDataSize);
    }
    mpStreamInfo->streamCaps.absoluteFragmentTimes = FALSE;

    // Init2
    {
        PCHAR pIotCoreCredentialEndPoint = "cne66nccv56pg.credentials.iot.ca-central-1.amazonaws.com";
        PCHAR pIotCoreCert = "/tmp/cert";
        PCHAR pIotCorePrivateKey = "/tmp/privkey";
        PCHAR pCaCert = "/tmp/rootca.pem";
        PCHAR pIotCoreRoleAlias = "KvsCameraIoTRoleAlias";
        PCHAR pThingName = "db-B813329BB08C";
        PCHAR pRegion = "ca-central-1";
        createDefaultCallbacksProviderWithIotCertificate(pIotCoreCredentialEndPoint, pIotCoreCert, pIotCorePrivateKey, pCaCert,
                                                         pIotCoreRoleAlias, pThingName, pRegion, NULL, NULL, &mpClientCallbacks);
    }
    STATUS retStatus = STATUS_SUCCESS;
    if (NULL != getenv(ENABLE_FILE_LOGGING)) {
        if ((retStatus = addFileLoggerPlatformCallbacksProvider(mpClientCallbacks, FILE_LOGGING_BUFFER_SIZE, MAX_NUMBER_OF_LOG_FILES,
                                                                (PCHAR) FILE_LOGGER_LOG_FILE_DIRECTORY_PATH, TRUE) != STATUS_SUCCESS)) {
            printf("File logging enable option failed with 0x%08x error code\n", retStatus);
        }
    }
    // Init3
    {
        createStreamCallbacks(&mpStreamCallbacks);
        addStreamCallbacks(mpClientCallbacks, mpStreamCallbacks);
    }
    // Init4
    createKinesisVideoClient(mpDeviceInfo, mpClientCallbacks, &mClientHandle);
    createKinesisVideoStreamSync(mClientHandle, mpStreamInfo, mpStreamHandle);//&mStreamHandle);
    return 0;
}
int KVSProducer_Deinit() {

    stopKinesisVideoStreamSync(*mpStreamHandle);
    freeDeviceInfo(&mpDeviceInfo);
    freeStreamInfoProvider(&mpStreamInfo);
    freeKinesisVideoStream(&mStreamHandle);
    freeKinesisVideoClient(&mClientHandle);

    int i = 0;
    for (i = 0; i < NUMBER_OF_AUDIO_FRAME_FILES; ++i) {
        SAFE_MEMFREE(gStreamSource->audioFrames[i].buffer);
    }

    for (i = 0; i < NUMBER_OF_VIDEO_FRAME_FILES; ++i) {
        SAFE_MEMFREE(gStreamSource->videoFrames[i].buffer);
    }

    freeCallbacksProvider(&mpClientCallbacks);
    return 0;
}
/*
int KVSProducer_PutFrame() { return 0; }
*/

PVOID putVideoFrameRoutine(PVOID args)
{
    STATUS retStatus = STATUS_SUCCESS;
    //PSampleCustomData data = (PSampleCustomData) args;
    Frame frame;
    UINT32 fileIndex = 0;
    STATUS status;
    UINT64 runningTime;
    DOUBLE startUpLatency;

    CHK(gStreamSource != NULL, STATUS_NULL_ARG);

    StreamSource_GetVideoFrame(fileIndex, &frame.frameData, &frame.size);
    //frame.frameData = gStreamSource->videoFrames[fileIndex].buffer;
    //frame.size = gStreamSource->videoFrames[fileIndex].size;
    frame.version = FRAME_CURRENT_VERSION;
    frame.trackId = DEFAULT_VIDEO_TRACK_ID;
    frame.duration = 0;
    frame.decodingTs = 0;
    frame.presentationTs = 0;
    frame.index = 0;

    // video track is used to mark new fragment. A new fragment is generated for every frame with FRAME_FLAG_KEY_FRAME
    frame.flags = fileIndex % DEFAULT_KEY_FRAME_INTERVAL == 0 ? FRAME_FLAG_KEY_FRAME : FRAME_FLAG_NONE;

    while (GETTIME() < gStreamSource->streamStopTime) {
        //status = putKinesisVideoFrame(data->streamHandle, &frame);
        //putKinesisVideoFrame(gStreamSource->streamHandle, &frame);
        status = putKinesisVideoFrame(gStreamSource->streamHandle, &frame);
        if (gStreamSource->firstFrame) {
            startUpLatency = (DOUBLE) (GETTIME() - gStreamSource->startTime) / (DOUBLE) HUNDREDS_OF_NANOS_IN_A_MILLISECOND;
            DLOGD("Start up latency: %lf ms", startUpLatency);
            gStreamSource->firstFrame = FALSE;
        } else if (frame.flags == FRAME_FLAG_KEY_FRAME && gEventsEnabled) {
            // generate an image and notification event at the start of the video stream.
            //putKinesisVideoEventMetadata(data->streamHandle, STREAM_EVENT_TYPE_NOTIFICATION | STREAM_EVENT_TYPE_IMAGE_GENERATION, NULL);
            putKinesisVideoEventMetadata(gStreamSource->streamHandle, STREAM_EVENT_TYPE_NOTIFICATION | STREAM_EVENT_TYPE_IMAGE_GENERATION, NULL);
            // only push this once in this sample. A customer may use this whenever it is necessary though
            gEventsEnabled = 0;
        }

        if (STATUS_SUCCEEDED(status)) {
            ATOMIC_STORE_BOOL(&gStreamSource->firstVideoFramePut, TRUE);
        }

        //ATOMIC_STORE_BOOL(&data->firstVideoFramePut, TRUE);
        if (STATUS_FAILED(status)) {
            printf("putKinesisVideoFrame failed with 0x%08x\n", status);
            status = STATUS_SUCCESS;
        }

        frame.presentationTs += SAMPLE_VIDEO_FRAME_DURATION;
        frame.decodingTs = frame.presentationTs;
        frame.index++;

        fileIndex = (fileIndex + 1) % NUMBER_OF_VIDEO_FRAME_FILES;
        frame.flags = fileIndex % DEFAULT_KEY_FRAME_INTERVAL == 0 ? FRAME_FLAG_KEY_FRAME : FRAME_FLAG_NONE;
        StreamSource_GetVideoFrame(fileIndex, &frame.frameData, &frame.size);
        // frame.frameData = data->videoFrames[fileIndex].buffer;
        //frame.size = data->videoFrames[fileIndex].size;

        // synchronize putKinesisVideoFrame to running time
        runningTime = GETTIME() - gStreamSource->streamStartTime;
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

PVOID putAudioFrameRoutine(PVOID args)
{
    STATUS retStatus = STATUS_SUCCESS;
    Frame frame;
    UINT32 fileIndex = 0;
    STATUS status;
    UINT64 runningTime;

    CHK(gStreamSource != NULL, STATUS_NULL_ARG);

    StreamSource_GetAudioFrame(fileIndex, &frame.frameData, &frame.size);
    frame.version = FRAME_CURRENT_VERSION;
    frame.trackId = DEFAULT_AUDIO_TRACK_ID;
    frame.duration = 0;
    frame.decodingTs = 0;     // relative time mode
    frame.presentationTs = 0; // relative time mode
    frame.index = 0;
    frame.flags = FRAME_FLAG_NONE; // audio track is not used to cut fragment

    while (GETTIME() < gStreamSource->streamStopTime) {
        // no audio can be put until first video frame is put
        if (ATOMIC_LOAD_BOOL(&gStreamSource->firstVideoFramePut)) {
            status = putKinesisVideoFrame(gStreamSource->streamHandle, &frame);
            if (STATUS_FAILED(status)) {
                printf("putKinesisVideoFrame for audio failed with 0x%08x\n", status);
                status = STATUS_SUCCESS;
            }

            frame.presentationTs += SAMPLE_AUDIO_FRAME_DURATION;
            frame.decodingTs = frame.presentationTs;
            frame.index++;

            fileIndex = (fileIndex + 1) % NUMBER_OF_AUDIO_FRAME_FILES;
            //frame.frameData = data->audioFrames[fileIndex].buffer;
            //frame.size = data->audioFrames[fileIndex].size;
            StreamSource_GetAudioFrame(fileIndex, &frame.frameData, &frame.size);

            // synchronize putKinesisVideoFrame to running time
            runningTime = GETTIME() - gStreamSource->streamStartTime;
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

INT32 main(INT32 argc, CHAR* argv[])
{
    STATUS retStatus = STATUS_SUCCESS;
    TID audioSendTid, videoSendTid;
    UINT64 streamingDuration = DEFAULT_STREAM_DURATION;
    SampleCustomData data;
    MEMSET(&data, 0x00, SIZEOF(SampleCustomData));
    gStreamSource = &data;

    StreamSource_SetDataSource((PCHAR) "../samples");
    StreamSource_Init();

    PCHAR streamName = "SH20-eventStream-db-B813329BB08C";
    KVSProducer_SetStreamName(streamName);

    UINT64 streamStopTime = GETTIME() + streamingDuration;
    //gStreamSource->startTime = GETTIME();
    //gStreamSource->firstFrame = TRUE;
    KVSProducer_Init();

    //gStreamSource->streamStopTime = streamStopTime;
    //gStreamSource->streamHandle = mStreamHandle;
    //gStreamSource->streamStartTime = GETTIME();
    StreamSource_SetHandler(mStreamHandle);
    StreamSource_Init_Time(TRUE, GETTIME(), streamStopTime, GETTIME());
    ATOMIC_STORE_BOOL(&gStreamSource->firstVideoFramePut, FALSE);

    THREAD_CREATE(&videoSendTid, putVideoFrameRoutine, (PVOID) &gStreamSource);
    THREAD_CREATE(&audioSendTid, putAudioFrameRoutine, (PVOID) &gStreamSource);

    THREAD_JOIN(videoSendTid, NULL);
    THREAD_JOIN(audioSendTid, NULL);

CleanUp:
    StreamSource_Deinit();
    KVSProducer_Deinit();
    return (INT32) retStatus;
}
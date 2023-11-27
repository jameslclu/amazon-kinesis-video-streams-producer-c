#include "KvsPlayer.h"
#include "ComponentProvider.h"
#include "KvsServiceConfig.h"
#include "MLogger.h"

static UINT64 gStartTime;
static UINT64 gStreamStartTime;
static UINT64 gStreamStopTime;
static volatile ATOMIC_BOOL gIsFirstVideoFramePut = FALSE;
STREAM_HANDLE mStreamHandle = INVALID_STREAM_HANDLE_VALUE;
static PSTREAM_HANDLE sPStreamHandle = &mStreamHandle;
static UINT8 gEventsEnabled = 0;
#define LIB_CONFIG_CA_PATH                     "/home/camera/kvs/rootca"
#define LIB_CONFIG_CERT_PATH                   "/tmp/cert"
#define LIB_CONFIG_PRIV_PATH                   "/tmp/privkey"
PCHAR pIotCoreCredentialEndPoint = "cne66nccv56pg.credentials.iot.ca-central-1.amazonaws.com";
PCHAR pIotCoreCert = "/home/camera/kvs/cert";
PCHAR pIotCorePrivateKey = "/home/camera/kvs/privkey";
PCHAR pCaCert = "/home/camera/kvs/rootca.pem";
PCHAR pIotCoreRoleAlias = "KvsCameraIoTRoleAlias";
PCHAR pThingName = "db-B813329BB08C";
PCHAR pRegion = "ca-central-1";
PStreamInfo pStreamInfo = NULL;
PTrackInfo spAudioTrack;
BYTE sAACAudioCpd[KVS_AAC_CPD_SIZE_BYTE];
PClientCallbacks pClientCallbacks = NULL;
PStreamCallbacks pStreamCallbacks = NULL;
CLIENT_HANDLE clientHandle = INVALID_CLIENT_HANDLE_VALUE;

PDeviceInfo sPDeviceInfo;
#define DEFAULT_STORAGE_SIZE              20 * 1024 * 1024
#define RECORDED_FRAME_AVG_BITRATE_BIT_PS 3800000
PCHAR sStreamName = "SH20-eventStream-db-B813329BB08C";

typedef struct {
    bool bFirstFrame;
    char StartFrameTimeUTCTs[ 64 ];
    char LastFrameTimeUTCTs[ 64 ];

    //KvsStreamingFrame_t Stream;
    Frame frame;

    INT64 prevPTS;
} KvsProducerFrameConfig_t;
#define AMBA_PACKET_PTS_SCALE 90000
BYTE aacAudioCpd[KVS_AAC_CPD_SIZE_BYTE];
//STREAM_HANDLE streamHandle = INVALID_STREAM_HANDLE_VALUE;

static PVOID putVideoFrameRoutine(PVOID args) {
    int64_t _targtpts = 0L;
    STATUS status;
    UINT64 pts = 0;
    static Frame frame;
    //memset(&frame, 0x00, sizeof (Frame));
    frame.index = 0;
    frame.trackId = DEFAULT_VIDEO_TRACK_ID;
    frame.duration = HUNDREDS_OF_NANOS_IN_A_SECOND / DEFAULT_FPS_VALUE;
    frame.version = FRAME_CURRENT_VERSION;
    frame.decodingTs = 0;
    frame.presentationTs = 0;
    uint32_t frameID = 0;

    MLogger::LOG(Level::DEBUG, "putKinesisVideoFrame: +");
    while( 0 == ComponentProvider::GetInstance()->GetStreamSource(FAKE)->
                GetVideoFrame(&frame.frameData, &frame.size, &pts) ) {
        // Put it into KvsRender();
        frame.index = frameID;
        frame.flags = frameID % DEFAULT_KEY_FRAME_INTERVAL == 0 ? FRAME_FLAG_KEY_FRAME : FRAME_FLAG_NONE;
        status = ComponentProvider::GetInstance()->GetKvsRender(RenderType::AWSPRODUCER)->PutVideoFrame(&frame);
        usleep(frame.duration/HUNDREDS_OF_NANOS_IN_A_MICROSECOND);
        //        if (i == 0) {
        //            status = putKinesisVideoEventMetadata(sStreamHandle, STREAM_EVENT_TYPE_NOTIFICATION | STREAM_EVENT_TYPE_IMAGE_GENERATION, NULL);
        //            MLogger::LOG(Level::DEBUG, "putKinesisVideoEventMetadata: status = %X", status);
        //        }
        frame.decodingTs += frame.duration;
        frame.presentationTs = frame.decodingTs;
        MLogger::LOG(Level::DEBUG, "putKinesisVideoFrame, (index=%d), status = %X", frameID, status);
        frameID++;
    }

    MLogger::LOG(Level::DEBUG, "putKinesisVideoFrame: -");
    return 0;
}

// The sample is corrent. let's frozen it.
static PVOID putVideoFrameRoutineDone(PVOID args) {
    //FmspFramePlaybackInfo_u _package;
    //KvsProducerFrameConfig_t video;
    int64_t _targtpts = 0L;
    STATUS status;

    // step 1: createRealtimeAudioVideoStreamInfoProviderWithCodecs();
    // video.frame.presentationTs = 0;
    // video.prevPTS = 0;
    // video.bFirstFrame = true;
    //MLogger::LOG(Level::DEBUG, "putVideoFrameRoutine: +");
    // Step 0:
    createDefaultDeviceInfo(&sPDeviceInfo);
    sPDeviceInfo->clientInfo.loggerLogLevel = LOG_LEVEL_DEBUG;
    sPDeviceInfo->storageInfo.storageSize = DEFAULT_STORAGE_SIZE;

    // Step 1:
    status = createOfflineVideoStreamInfoProviderWithCodecs(sStreamName, DEFAULT_RETENTION_PERIOD, DEFAULT_BUFFER_DURATION, VIDEO_CODEC_ID_H264,
                                                    &pStreamInfo);
    MLogger::LOG(Level::DEBUG, "createDefaultCallbacksProviderWithIotCertificate: %X", status);
    status = setStreamInfoBasedOnStorageSize(DEFAULT_STORAGE_SIZE, RECORDED_FRAME_AVG_BITRATE_BIT_PS, 1, pStreamInfo);
    MLogger::LOG(Level::DEBUG, "setStreamInfoBasedOnStorageSize: %X", status);
//    createRealtimeAudioVideoStreamInfoProviderWithCodecs(
//            sStreamName,
//            DEFAULT_RETENTION_PERIOD, DEFAULT_BUFFER_DURATION,
//            VIDEO_CODEC_ID_H264, AUDIO_CODEC_ID_AAC,
//            &spStreamInfo);
//    if (DEFAULT_AUDIO_TRACK_ID == spStreamInfo->streamCaps.trackInfoList[ 0 ].trackId ) {
//        spAudioTrack = &spStreamInfo->streamCaps.trackInfoList[0];
//    } else {
//        spAudioTrack = &spStreamInfo->streamCaps.trackInfoList[1];
//    }
//    spAudioTrack->codecPrivateData = sAACAudioCpd;
//    spAudioTrack->codecPrivateDataSize = KVS_AAC_CPD_SIZE_BYTE;

    // step 2: mkvgenGenerateAacCpd();
//    mkvgenGenerateAacCpd( AAC_LC,
//                          AAC_AUDIO_TRACK_SAMPLING_RATE,
//                          AAC_AUDIO_TRACK_CHANNEL_CONFIG,
//                          spAudioTrack->codecPrivateData,
//                          spAudioTrack->codecPrivateDataSize);
    pStreamInfo->streamCaps.absoluteFragmentTimes = FALSE;
    // ToDo: figure the stattime value: p->StartTime = GETTIME();
    //pStreamInfo->streamCaps.absoluteFragmentTimes = FALSE;
    //p->Control.pStreamInfo->streamCaps.absoluteFragmentTimes = FALSE;
    // step 3: createDefaultCallbacksProviderWithIotCertificate();
    status = createDefaultCallbacksProviderWithIotCertificate(pIotCoreCredentialEndPoint, pIotCoreCert, pIotCorePrivateKey,
                                                              pCaCert, pIotCoreRoleAlias, pThingName, pRegion,
                                                              NULL, NULL, &pClientCallbacks);
    MLogger::LOG(Level::DEBUG, "createDefaultCallbacksProviderWithIotCertificate: %X", status);

    // step 4: addFileLoggerPlatformCallbacksProvider();
    STATUS retStatus = STATUS_SUCCESS;
    if (NULL != getenv(ENABLE_FILE_LOGGING)) {
        if ((retStatus = addFileLoggerPlatformCallbacksProvider(pClientCallbacks, FILE_LOGGING_BUFFER_SIZE, MAX_NUMBER_OF_LOG_FILES,
                                                            (PCHAR) "/data/tmp/middleware/", TRUE) != STATUS_SUCCESS)) {
            printf("File logging enable option failed with 0x%08x error code\n", retStatus);
        }
    }

    // step 5: createStreamCallbacks();
    MLogger::LOG(Level::DEBUG, "Create Stream Callbacks: %X", createStreamCallbacks(&pStreamCallbacks));

    // step 6: addStreamCallbacks();
    MLogger::LOG(Level::DEBUG, "Add Stream Callbacks: %X", addStreamCallbacks(pClientCallbacks, pStreamCallbacks));

    // step 7: createKinesisVideoClient();
    status = createKinesisVideoClient(sPDeviceInfo, pClientCallbacks, &clientHandle);
    MLogger::LOG(Level::DEBUG, "7. Create Kinesis Video Client: result = %X", status);

    // step 8: createKinesisVideoStreamSync();
    status = createKinesisVideoStreamSync(clientHandle, pStreamInfo, sPStreamHandle);

    //CHK_STATUS(createKinesisVideoStreamSync(clientHandle, pStreamInfo, &streamHandle));
    MLogger::LOG(Level::DEBUG, "8. Create Kinesis Video Stream Sync: result = %X", status);
    UINT64 pts = 0;
    static Frame frame;
    //memset(&frame, 0x00, sizeof (Frame));
    frame.index = 0;
    frame.trackId = DEFAULT_VIDEO_TRACK_ID;
    frame.duration = HUNDREDS_OF_NANOS_IN_A_SECOND / DEFAULT_FPS_VALUE;
    frame.version = FRAME_CURRENT_VERSION;
    frame.decodingTs = 0;
    frame.presentationTs = 0;
    uint32_t frameID = 0;

    while( 0 == ComponentProvider::GetInstance()->GetStreamSource(FAKE)->
              GetVideoFrame(&frame.frameData, &frame.size, &pts) ) {
        //static int i = 0;
        // frame.presentationTs += frame.duration;
        frame.index = frameID;
        frame.flags = frameID % DEFAULT_KEY_FRAME_INTERVAL == 0 ? FRAME_FLAG_KEY_FRAME : FRAME_FLAG_NONE;
        status = putKinesisVideoFrame( *sPStreamHandle, &frame);

        usleep(frame.duration/HUNDREDS_OF_NANOS_IN_A_MICROSECOND);
//        if (i == 0) {
//            status = putKinesisVideoEventMetadata(sStreamHandle, STREAM_EVENT_TYPE_NOTIFICATION | STREAM_EVENT_TYPE_IMAGE_GENERATION, NULL);
//            MLogger::LOG(Level::DEBUG, "putKinesisVideoEventMetadata: status = %X", status);
//        }
        frame.decodingTs += frame.duration;
        frame.presentationTs = frame.decodingTs;
        MLogger::LOG(Level::DEBUG, "putKinesisVideoFrame, (index=%d), status = %X", frameID, status);
        frameID++;
    }

    MLogger::LOG(Level::DEBUG, "putKinesisVideoFrame");
    return 0;
}
/*
static PVOID putVideoFrameRoutine(PVOID args) {
    FmspFramePlaybackInfo_u _package;
    KvsProducerFrameConfig_t video;
    int64_t _targtpts = 0L;

    // step 1: createRealtimeAudioVideoStreamInfoProviderWithCodecs();
    video.frame.presentationTs = 0;
    video.prevPTS = 0;
    video.bFirstFrame = true;
    MLogger::LOG(Level::DEBUG, "putVideoFrameRoutine: +");
    createDefaultDeviceInfo(&sPDeviceInfo);
    sPDeviceInfo->clientInfo.loggerLogLevel = LOG_LEVEL_DEBUG;

    createRealtimeVideoStreamInfoProviderWithCodecs(sStreamName, DEFAULT_RETENTION_PERIOD, DEFAULT_BUFFER_DURATION, VIDEO_CODEC_ID_H264,
                                                               &spStreamInfo);

    // To specify PCM/G.711 use createRealtimeAudioStreamInfoProviderWithCodecs
    // adjust members of pStreamInfo here if needed

    // set up audio cpd.
    spAudioTrack = spStreamInfo->streamCaps.trackInfoList[0].trackId == 1 ? &spStreamInfo->streamCaps.trackInfoList[0]
                                                                        : &spStreamInfo->streamCaps.trackInfoList[1];
    spAudioTrack->codecPrivateData = aacAudioCpd;
    spAudioTrack->codecPrivateDataSize = KVS_AAC_CPD_SIZE_BYTE;
    //mkvgenGenerateAacCpd(AAC_LC, AAC_AUDIO_TRACK_SAMPLING_RATE, AAC_AUDIO_TRACK_CHANNEL_CONFIG, spAudioTrack->codecPrivateData,
//                         spAudioTrack->codecPrivateDataSize);
//    spStreamInfo->streamCaps.absoluteFragmentTimes = FALSE;
    //createRealtimeAudioStreamInfoProviderWithCodecs
//TBD
//    createRealtimeAudioVideoStreamInfoProviderWithCodecs(
//            sStreamName,
//            DEFAULT_RETENTION_PERIOD, DEFAULT_BUFFER_DURATION,
//            VIDEO_CODEC_ID_H264, AUDIO_CODEC_ID_AAC,
//            &spStreamInfo);
//    if (DEFAULT_AUDIO_TRACK_ID == spStreamInfo->streamCaps.trackInfoList[ 0 ].trackId ) {
//        spAudioTrack = &spStreamInfo->streamCaps.trackInfoList[0];
//    } else {
//        spAudioTrack = &spStreamInfo->streamCaps.trackInfoList[1];
//    }
//    spAudioTrack->codecPrivateData = sAACAudioCpd;
//    spAudioTrack->codecPrivateDataSize = KVS_AAC_CPD_SIZE_BYTE;

    // step 2: mkvgenGenerateAacCpd();
    mkvgenGenerateAacCpd( AAC_LC,
                            AAC_AUDIO_TRACK_SAMPLING_RATE,
                            AAC_AUDIO_TRACK_CHANNEL_CONFIG,
                            spAudioTrack->codecPrivateData,
                            spAudioTrack->codecPrivateDataSize);
    spStreamInfo->streamCaps.absoluteFragmentTimes = FALSE;
    // ToDo: figure the stattime value: p->StartTime = GETTIME();

    // step 3: createDefaultCallbacksProviderWithIotCertificate();
    int d = createDefaultCallbacksProviderWithIotCertificate(pIotCoreCredentialEndPoint, pIotCoreCert, pIotCorePrivateKey, pCaCert,
                                                     pIotCoreRoleAlias, pThingName, pRegion, NULL, NULL, &spClientCallbacks);
    MLogger::LOG(Level::DEBUG, "createDefaultCallbacksProviderWithIotCertificate: %d", d);
    // step 4: addFileLoggerPlatformCallbacksProvider();
    STATUS retStatus = STATUS_SUCCESS;
    //if (NULL != getenv(ENABLE_FILE_LOGGING)) {
        if ((retStatus = addFileLoggerPlatformCallbacksProvider(spClientCallbacks, FILE_LOGGING_BUFFER_SIZE, MAX_NUMBER_OF_LOG_FILES,
                                                                (PCHAR) "/data/tmp/middleware/", TRUE) != STATUS_SUCCESS)) {
            printf("File logging enable option failed with 0x%08x error code\n", retStatus);
        }
    //}

    // step 5: createStreamCallbacks();
    MLogger::LOG(Level::DEBUG, "Create Stream Callbacks: %d", createStreamCallbacks(&spStreamCallbacks));

    // step 6: addStreamCallbacks();
    MLogger::LOG(Level::DEBUG, "Add Stream Callbacks: %d", addStreamCallbacks(spClientCallbacks, spStreamCallbacks));

    // step 7: createKinesisVideoClient();
//    if (sPDeviceInfo == NULL) {
//        MLogger::LOG(Level::ERROR, "sPDeviceInfo == NULL");
//    }
//
//    if (spClientCallbacks == NULL) {
//        MLogger::LOG(Level::ERROR, "spClientCallbacks == NULL");
//    }
//
//    if(sPClientHandle == NULL) {
//        MLogger::LOG(Level::ERROR, "sPClientHandle == NULL");
//    }
    int result = createKinesisVideoClient(sPDeviceInfo, spClientCallbacks, sPClientHandle);
    MLogger::LOG(Level::DEBUG, "7. Create Kinesis Video Client: result = %d", result);

    // step 8: createKinesisVideoStreamSync();
//    if (NULL == *sPClientHandle) {
//        MLogger::LOG(Level::ERROR, "sPClientHandle == NULL");
//    }
//    if (sPStreamHandle == NULL) {
//        MLogger::LOG(Level::ERROR, "sPStreamHandle == NULL");
//    }

    result = createKinesisVideoStreamSync(*sPClientHandle, spStreamInfo, &streamHandle);//&mStreamHandle);
    //CHK_STATUS(createKinesisVideoStreamSync(clientHandle, pStreamInfo, &streamHandle));
    MLogger::LOG(Level::DEBUG, "8. Create Kinesis Video Stream Sync: result = %d", result);

    int32_t _deltaPts = 0;
    int64_t _firstVideoPTS = 0;
    bool _isFirstFrame = true;
    static Frame _frame;
    _frame.version = FRAME_CURRENT_VERSION;
    _frame.trackId = DEFAULT_VIDEO_TRACK_ID;
    _frame.duration = 0;
    _frame.decodingTs = 0;
    _frame.presentationTs = 0;
    _frame.index = 0;
    sleep(2);
    while(true) {
        if (EXIT_SUCCESS == OryxStreamingGetFrame(&_package)) {
            if (FMSP_PACKET_TYPE_VIDEO == _package.data.Type) {
                MLogger::LOG(Level::DEBUG, "_package.data.pts = %d, _targtpts=%d", _package.data.pts, _targtpts);
                static STATUS status;

                if (video.bFirstFrame) {
                    _firstVideoPTS = _package.data.pts;
                    video.bFirstFrame = false;
                }


                    _frame.presentationTs = (_firstVideoPTS - _package.data.pts) * 111;
                    _frame.decodingTs = _frame.presentationTs;
                    _frame.index ++;
                    MLogger::LOG(Level::DEBUG, "packatge: presentationTs=%d, decodingTs=%d, _deltaPts=%d", video.frame.presentationTs, _package.data.Type, _deltaPts);


                    _frame.frameData = &_package.data.Buffer[0];
                    _frame.size = _package.data.Size;
                    MLogger::LOG(Level::DEBUG, "_frame.size=%d", _frame.size);


                    MLogger::LOG(Level::DEBUG, "--> putKinesisVideoFrame");
                    status = putKinesisVideoFrame( *sPClientHandle, &_frame);
                    MLogger::LOG(Level::DEBUG, "<-- putKinesisVideoFrame");
                    MLogger::LOG(Level::INFO, "putKinesisVideoFrame: status=%d", status);
            }
        } else {
            MLogger::LOG(Level::DEBUG, "Failed to get frame");
        }
        sleep(1);
    }
    return 0;
}
*/
/*
static PVOID putVideoFrameRoutine2(PVOID args)
{
    STATUS retStatus = STATUS_SUCCESS;
    Frame frame;
    UINT32 fileIndex = 0;
    STATUS status;
    UINT64 runningTime;
    DOUBLE startUpLatency;
    int64_t pts;

    ComponentProvider::GetInstance()->GetStreamSource(FAKE)
        ->GetVideoFrame(&frame.frameData, &frame.size, &pts);

    frame.version = FRAME_CURRENT_VERSION;
    frame.trackId = DEFAULT_VIDEO_TRACK_ID;
    frame.duration = 0;
    frame.decodingTs = 0;
    frame.presentationTs = 0;
    frame.index = 0;
    //int64_t pts = 0;

    // video track is used to mark new fragment. A new fragment is generated for every frame with FRAME_FLAG_KEY_FRAME
    frame.flags = fileIndex % DEFAULT_KEY_FRAME_INTERVAL == 0 ? FRAME_FLAG_KEY_FRAME : FRAME_FLAG_NONE;

    while (GETTIME() < gStreamStopTime) {
        //status = putKinesisVideoFrame(*mpStreamHandle, &frame);
        status = ComponentProvider::GetInstance()->GetKvsRender(AWSPRODUCKER)
                     ->PutVideoFrame(0, &frame);
        if (gIsFirstVideoFramePut) {
            startUpLatency = (DOUBLE) (GETTIME() - gStartTime) / (DOUBLE) HUNDREDS_OF_NANOS_IN_A_MILLISECOND;
            DLOGD("Start up latency: %lf ms", startUpLatency);
            //psStreamSource->firstFrame = FALSE;
            gIsFirstVideoFramePut = FALSE;
        } else if (frame.flags == FRAME_FLAG_KEY_FRAME && gEventsEnabled) {
            // generate an image and notification event at the start of the video stream.
            //putKinesisVideoEventMetadata(data->streamHandle, STREAM_EVENT_TYPE_NOTIFICATION | STREAM_EVENT_TYPE_IMAGE_GENERATION, NULL);
            putKinesisVideoEventMetadata(*sPStreamHandle, STREAM_EVENT_TYPE_NOTIFICATION | STREAM_EVENT_TYPE_IMAGE_GENERATION, NULL);
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
        ComponentProvider::GetInstance()->GetStreamSource(FAKE)->GetVideoFrame(&frame.frameData, &frame.size, &pts);

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
*/
/*
static PVOID putAudioFrameRoutine(PVOID args)
{
    STATUS retStatus = STATUS_SUCCESS;
    Frame frame;
    UINT32 fileIndex = 0;
    STATUS status;
    UINT64 runningTime;
    int64_t pts = 0;

    ComponentProvider::GetInstance()->GetStreamSource(FAKE)
        ->GetAudioFrame(&frame.frameData, &frame.size, &pts);

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
                ->PutVideoFrame(0, &frame);
            //status = putKinesisVideoFrame(*mpStreamHandle, &frame);
            if (STATUS_FAILED(status)) {
                printf("putKinesisVideoFrame for audio failed with 0x%08x\n", status);
                status = STATUS_SUCCESS;
            }

            frame.presentationTs += SAMPLE_AUDIO_FRAME_DURATION;
            frame.decodingTs = frame.presentationTs;
            frame.index++;

            fileIndex = (fileIndex + 1) % NUMBER_OF_AUDIO_FRAME_FILES;
            int64_t pts = 0;
            //ComponentProvider::GetInstance()->GetStreamSource(FAKE)->GetAudioFrame(&frame.frameData, &frame.size, &pts);


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
*/
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
    //ComponentProvider::GetInstance()->GetKvsRender(AWSPRODUCKER)->BaseInit();
    //gStreamStartTime = GETTIME();
    //gStartTime = gStreamStartTime;
    //gStreamStopTime = gStreamStartTime + DEFAULT_STREAM_DURATION;
    //gIsFirstVideoFramePut = FALSE;
    MLogger::LOG(Level::DEBUG, "KvsPlayer::Start");
    THREAD_CREATE(&videoSendTid, putVideoFrameRoutine, NULL);
    //THREAD_CREATE(&audioSendTid, putAudioFrameRoutine, NULL);

    //THREAD_JOIN(videoSendTid, nullptr);
    //THREAD_JOIN(audioSendTid, nullptr);
    return 0;
}

int KvsPlayer::Pause() {
    return 0;
}

int KvsPlayer::Stop() {
    ComponentProvider::GetInstance()->GetKvsRender(RenderType::AWSPRODUCER)->BaseDeinit();
    return 0;
}

//int GetStatus(KvsPlayerState state);
//#include "KvsPlayer.h"
#include "KvsPlayer.h"
#include "ComponentProvider.h"
#include "KvsServiceConfig.h"
#include "MLogger.h"

volatile ATOMIC_BOOL firstVideoFramePut = false;
UINT64 streamStartTime;
UINT64 streamStopTime;
DOUBLE startUpLatency;
UINT64 startTime = GETTIME();

static PVOID putAVFrameRoutine(PVOID args) {
    STATUS status;
    UINT64 pts = 0;
    static Frame frame;
    UINT64 runningTime;
    UINT32 fileIndex = 0;
#ifdef CV28_BUILD
    ComponentProvider::GetInstance()->GetStreamSource(ORYX)->GetVideoFrame(&frame.frameData, &frame.size, &pts);


#endif
    frame.version = FRAME_CURRENT_VERSION;
    frame.trackId = DEFAULT_VIDEO_TRACK_ID;
#ifdef CONFIG_VIDEO_AUDIO_BOTH
    // Confirmed
    frame.duration = 0;
#else
#ifdef CONFIG_AUDIO_ONLY
    frame.duration = 0;
#else CONFIG_VIDEO_ONLY
    // Confirmed
    frame.duration = HUNDREDS_OF_NANOS_IN_A_SECOND / DEFAULT_FPS_VALUE;
#endif
#endif
    frame.decodingTs = 0;
    frame.presentationTs = 0;
    frame.index = 0;
    uint32_t frameID = 0;
    frame.flags = fileIndex % DEFAULT_KEY_FRAME_INTERVAL == 0 ? FRAME_FLAG_KEY_FRAME : FRAME_FLAG_NONE;
    MLogger::LOG(Level::DEBUG, "putVideoFrameRoutine: +");

    while (GETTIME() < streamStopTime) {
        status = ComponentProvider::GetInstance()->GetKvsRender(RenderType::AWSPRODUCER)->PutVideoFrame(&frame);
        if (!firstVideoFramePut) {
            startUpLatency = (DOUBLE) (GETTIME() - startTime) / (DOUBLE) HUNDREDS_OF_NANOS_IN_A_MILLISECOND;
            DLOGD("Start up latency: %lf ms", startUpLatency);
            firstVideoFramePut = true;
        }
        frame.presentationTs += SAMPLE_VIDEO_FRAME_DURATION;
        frame.decodingTs = frame.presentationTs;
        frame.index++;

        frame.flags = frame.index % DEFAULT_KEY_FRAME_INTERVAL == 0 ? FRAME_FLAG_KEY_FRAME : FRAME_FLAG_NONE;;
        ComponentProvider::GetInstance()->GetStreamSource(FAKE)->GetVideoFrame(&frame.frameData, &frame.size, &pts);

        runningTime = GETTIME() - streamStartTime;
        if (runningTime < frame.presentationTs) {
            THREAD_SLEEP((frame.presentationTs - runningTime) * 0.9);
        }
        frameID++;
    }
    MLogger::LOG(Level::DEBUG, "putVideoFrameRoutine: -");
    return 0;
}

static PVOID putAudioFrameRoutine(PVOID args) {
    STATUS status;
    UINT64 pts = 0;
    static Frame frame;
    frame.version = FRAME_CURRENT_VERSION;
#ifdef CONFIG_AUDIO_ONLY
    frame.trackId = DEFAULT_AUDIO_ONLY_TRACK_ID;
#else
    frame.trackId = DEFAULT_AUDIO_TRACK_ID;
#endif
    frame.duration = 0;
    frame.decodingTs = 0;
    frame.presentationTs = 0;
    frame.index = 0;
#ifdef CONFIG_AUDIO_ONLY
    frame.flags = FRAME_FLAG_KEY_FRAME;
#else
    frame.flags = FRAME_FLAG_NONE; // Audio-only: FRAME_FLAG_KEY_FRAME;
#endif
    UINT64 runningTime;
    MLogger::LOG(Level::DEBUG, "putAudioFrameRoutine: +");
    while( GETTIME() < streamStopTime) {
        if (firstVideoFramePut) {
#ifdef CV28_BUILD
            ComponentProvider::GetInstance()->GetStreamSource(ORYX)->GetAudioFrame(&frame.frameData, &frame.size, &pts);
#else
            ComponentProvider::GetInstance()->GetStreamSource(FAKE)->GetAudioFrame(&frame.frameData, &frame.size, &pts);
#endif
            status = ComponentProvider::GetInstance()->GetKvsRender(RenderType::AWSPRODUCER)->PutAudioFrame(&frame);
            if (STATUS_FAILED(status)) {
                //printf("putKinesisVideoFrame for audio failed with 0x%08x\n", status);
                MLogger::LOG(Level::ERROR, "putAudioFrameRoutine, (index=%d), status = 0x%08x", frame.index, status);
            } else {
                //MLogger::LOG(Level::DEBUG, "putAudioFrameRoutine, (index=%d), status = 0x%08x", frame.index, status);
            }

            frame.presentationTs += SAMPLE_AUDIO_FRAME_DURATION;
            frame.decodingTs = frame.presentationTs;
            frame.index++;

            // synchronize putKinesisVideoFrame to running time
            runningTime = GETTIME() - streamStartTime;
            if (runningTime < frame.presentationTs) {
                THREAD_SLEEP(frame.presentationTs - runningTime);
            }
        }

    }

    MLogger::LOG(Level::DEBUG, "putAudioFrameRoutine: -");
    return 0;
}

static PVOID putVideoFrameRoutine(PVOID args) {
    STATUS status;
    UINT64 pts = 0;
    static Frame frame;
    UINT64 runningTime;
    UINT32 fileIndex = 0;
#ifdef CV28_BUILD
    ComponentProvider::GetInstance()->GetStreamSource(ORYX)->GetVideoFrame(&frame.frameData, &frame.size, &pts);
#else
    ComponentProvider::GetInstance()->GetStreamSource(FAKE)->GetVideoFrame(&frame.frameData, &frame.size, &pts);
#endif
    frame.version = FRAME_CURRENT_VERSION;
    frame.trackId = DEFAULT_VIDEO_TRACK_ID;
#ifdef CONFIG_VIDEO_AUDIO_BOTH
    // Confirmed
    frame.duration = 0;
#else
#ifdef CONFIG_AUDIO_ONLY
    frame.duration = 0;
#else CONFIG_VIDEO_ONLY
    // Confirmed
    frame.duration = HUNDREDS_OF_NANOS_IN_A_SECOND / DEFAULT_FPS_VALUE;
#endif
#endif
    frame.decodingTs = 0;
    frame.presentationTs = 0;
    frame.index = 0;
    uint32_t frameID = 0;
    frame.flags = fileIndex % DEFAULT_KEY_FRAME_INTERVAL == 0 ? FRAME_FLAG_KEY_FRAME : FRAME_FLAG_NONE;
    MLogger::LOG(Level::DEBUG, "putVideoFrameRoutine: +");

    while (GETTIME() < streamStopTime) {
        status = ComponentProvider::GetInstance()->GetKvsRender(RenderType::AWSPRODUCER)->PutVideoFrame(&frame);
        if (!firstVideoFramePut) {
            startUpLatency = (DOUBLE) (GETTIME() - startTime) / (DOUBLE) HUNDREDS_OF_NANOS_IN_A_MILLISECOND;
            DLOGD("Start up latency: %lf ms", startUpLatency);
            firstVideoFramePut = true;
        } else if (frame.flags == FRAME_FLAG_KEY_FRAME) {
            // generate an image and notification event at the start of the video stream.
            //putKinesisVideoEventMetadata(streamHandle, STREAM_EVENT_TYPE_NOTIFICATION | STREAM_EVENT_TYPE_IMAGE_GENERATION, NULL);
            //MLogger::LOG(Level::DEBUG, "putVideoFrameRoutine: FirstKeyFrarme: ");
            // only push this once in this sample. A customer may use this whenever it is necessary though
        }

        frame.presentationTs += SAMPLE_VIDEO_FRAME_DURATION;
        frame.decodingTs = frame.presentationTs;
        frame.index++;

        frame.flags = frame.index % DEFAULT_KEY_FRAME_INTERVAL == 0 ? FRAME_FLAG_KEY_FRAME : FRAME_FLAG_NONE;;
        ComponentProvider::GetInstance()->GetStreamSource(FAKE)->GetVideoFrame(&frame.frameData, &frame.size, &pts);

        runningTime = GETTIME() - streamStartTime;
        if (runningTime < frame.presentationTs) {
            // reduce sleep time a little for smoother video
            THREAD_SLEEP((frame.presentationTs - runningTime) * 0.9);
        }
        //MLogger::LOG(Level::DEBUG, "putKinesisVideoFrame, (index=%d), status = 0x%08x", frameID, status);
        frameID++;
    }
    MLogger::LOG(Level::DEBUG, "putVideoFrameRoutine: -");
    return 0;
}

// The sample is corrent. let's frozen it.
/*
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
*/

static TID audioSendTid, videoSendTid, avSentTid;
int KvsPlayer::HandleAsyncMethod(const MethodItem& method) {
    startTime = GETTIME();
    streamStartTime = GETTIME();
    streamStopTime = streamStartTime + DEFAULT_STREAM_DURATION;
    if ("Start" == method.m_method) {
#ifdef CV28_BUILD
        THREAD_CREATE(&avSentTid, putAVFrameRoutine, NULL);
        THREAD_JOIN(avSentTid, nullptr);
#else
        ComponentProvider::GetInstance()->GetStreamSource(FAKE)->Reset();
        ComponentProvider::GetInstance()->GetKvsRender(RenderType::AWSPRODUCER)->BaseInit();
        // Either of Video only or Audio Only: is avalable
#ifdef CONFIG_VIDEO_AUDIO_BOTH
        THREAD_CREATE(&videoSendTid, putVideoFrameRoutine, NULL);
        THREAD_CREATE(&audioSendTid, putAudioFrameRoutine, NULL);
        THREAD_JOIN(videoSendTid, nullptr);
        THREAD_JOIN(audioSendTid, nullptr);
#else
#ifdef CONFIG_VIDEO_ONLY
        THREAD_CREATE(&videoSendTid, putVideoFrameRoutine, NULL);
        THREAD_JOIN(videoSendTid, nullptr);
#elifdef CONFIG_AUDIO_ONLY
        THREAD_CREATE(&audioSendTid, putAudioFrameRoutine, NULL);
        THREAD_JOIN(audioSendTid, nullptr);
#endif
#endif
#endif
        ComponentProvider::GetInstance()->GetKvsRender(RenderType::AWSPRODUCER)->BaseDeinit();
    }

    return 0;
}

KvsPlayer::KvsPlayer(): ServiceBase("KvsPlayer") {}

KvsPlayer::~KvsPlayer() {}

int KvsPlayer::Init() {
    ServiceBaseInit();
    return 0;
}

int KvsPlayer::Deinit()
{
    ServiceBaseDeinit();
    return 0;
}

int KvsPlayer::Start() {
    MLogger::LOG(Level::DEBUG, "KvsPlayer::Start");
    EnqueuenMethod(MethodItem("Start", "str"));
    return 0;
}

int KvsPlayer::Pause() {
    return 0;
}

int KvsPlayer::Stop() {
    ComponentProvider::GetInstance()->GetKvsRender(RenderType::AWSPRODUCER)->BaseDeinit();
    return 0;
}
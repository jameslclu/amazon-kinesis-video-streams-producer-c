#include "KvsPlayer.h"
#include "ComponentProvider.h"
#include "KvsServiceConfig.h"
#include "MLogger.h"
#include "am_base_include.h"
#include "am_define.h"
#include "am_export_if.h"
#include "../pc/AllPCStructures.h"
static PCSampleCustomData sPCSampleCustomData;
MediaStreamConfig sEventConfig = {.serviceType = EVENT};
MediaStreamConfig sManualConfig = {.serviceType = MANUAL};

AV_CONFIG sAVCONFIG = IN_A_THREAD;
static int g_hw_timer_fd = -1;  ///< File descriptor for the hardware timer.
int64_t get_current_pts()
{
    uint8_t pts[32] = {0};
    int64_t current_pts = 0;
    if (g_hw_timer_fd < 0)
    {
        printf("get current pts, hw timer fd < 0");
        return -1;
    }
    if (read(g_hw_timer_fd, pts, sizeof(pts)) < 0)
    {
        printf("read");
    }
    else
    {
        current_pts = strtoll((const char *)pts, (char **)nullptr, 10);
    }
    return current_pts;
}

static PVOID deviceAVPlayThread(PVOID args) {
    STATUS status;
    UINT64 pts = 0;
    static Frame vFrame, aFrame;
    UINT64 runningTime;
    UINT32 fileIndex = 0;
    sManualConfig.start_pts = get_current_pts();
    sManualConfig.vFrame.version = FRAME_CURRENT_VERSION;
    sManualConfig.vFrame.trackId = DEFAULT_VIDEO_TRACK_ID;
    sManualConfig.vFrame.duration = 0;
    sManualConfig.vFrame.presentationTs = 0;
    sManualConfig.vFrame.decodingTs = 0;
    sManualConfig.vFrame.index = 0;

    sManualConfig.aFrame.version = FRAME_CURRENT_VERSION;
    sManualConfig.aFrame.trackId = DEFAULT_AUDIO_TRACK_ID;
    sManualConfig.aFrame.duration = 0;
    sManualConfig.aFrame.decodingTs = 0;     /*!< Relative time mode */
    sManualConfig.aFrame.presentationTs = 0; /*!< Relative time mode */
    sManualConfig.aFrame.index = 0;
    sManualConfig.aFrame.flags = FRAME_FLAG_NONE; /*!< audio track is not used to cut fragment */

//#ifdef CV28_BUILD
    AMExportPacket packet;
    AMExportPacket aPacket;
    AMExportPacket vPacket;
    MLogger::LOG(Level::DEBUG, "KvsPlayer::deviceAVPlayThread: +");
    while(true) {
#ifdef CONFIG_AV_IN_ONE_SOURCE
        if (ComponentProvider::GetInstance()->GetStreamSource(ORYX)->GetAVFrame(&packet) == 0) {
            //MLogger::LOG(Level::DEBUG, "KvsPlayer::putAVFrameRoutine: 1-1");
            if ((sManualConfig.last_seq_num > 0) && (packet.seq_num - sManualConfig.last_seq_num) != 1)
            {
                printf("ERROR: Some packets have been dropped\n");
            }

            if (sManualConfig.start_pts != 0 && packet.pts >= sManualConfig.start_pts)
            {
                ComponentProvider::GetInstance()->GetKvsRender(RenderType::AWSPRODUCER)
                    ->KvsProducerPutFrameRoutine(&sManualConfig, &packet);
            }
            ComponentProvider::GetInstance()->GetStreamSource(ORYX)->ReleaseAVFrame(&packet);
        } else {
            MLogger::LOG(Level::DEBUG, "KvsPlayer::deviceAVPlayThread: 1-2");
        }
#else
        if (ComponentProvider::GetInstance()->GetStreamSource(ORYX)->GetVFrame(&vPacket) == 0) {
            ComponentProvider::GetInstance()->GetKvsRender(RenderType::AWSPRODUCER)
                ->KvsProducerPutFrameRoutine(&sManualConfig, &vPacket);
        } else {
            MLogger::LOG(Level::ERROR, "KvsPlayer::GetVFrame: ERROR");
        }
        ComponentProvider::GetInstance()->GetStreamSource(ORYX)->ReleaseVFrame(&vPacket);

        if (ComponentProvider::GetInstance()->GetStreamSource(ORYX)->GetAFrame(&aPacket) == 0) {
            ComponentProvider::GetInstance()->GetKvsRender(RenderType::AWSPRODUCER)
                                ->KvsProducerPutFrameRoutine(&sManualConfig, &aPacket);
        } else {
            MLogger::LOG(Level::ERROR, "KvsPlayer::GetAFrame: ERROR");
        }
        ComponentProvider::GetInstance()->GetStreamSource(ORYX)->ReleaseAFrame(&aPacket);
#endif
    }
    MLogger::LOG(Level::DEBUG, "KvsPlayer::deviceAVPlayThread: -");
//#endif

    //MLogger::LOG(Level::DEBUG, "putAVFrameRoutine: +");

    //while (GETTIME() < streamStopTime) {
        //status = ComponentProvider::GetInstance()->GetKvsRender(RenderType::AWSPRODUCER)->PutVideoFrame(&frame);
        //if (!firstVideoFramePut) {
        //    startUpLatency = (DOUBLE) (GETTIME() - startTime) / (DOUBLE) HUNDREDS_OF_NANOS_IN_A_MILLISECOND;
        //    DLOGD("Start up latency: %lf ms", startUpLatency);
        //    firstVideoFramePut = true;
        //}
        //if (firstPTSSetted == false) {
        //    firstVideoPTS = pts;
        //    firstPTSSetted = true;
        //}
        //frame.presentationTs = (pts - firstVideoPTS) * 111;
        //frame.decodingTs = frame.presentationTs;
        //frame.index++;

        //frame.flags = frame.index % DEFAULT_KEY_FRAME_INTERVAL == 0 ? FRAME_FLAG_KEY_FRAME : FRAME_FLAG_NONE;;
        //ComponentProvider::GetInstance()->GetStreamSource(FAKE)->GetVideoFrame(&frame.frameData, &frame.size, &pts);

        //runningTime = GETTIME() - streamStartTime;
        //if (runningTime < frame.presentationTs) {
//            THREAD_SLEEP((frame.presentationTs - runningTime) * 0.9);
//        }
//        frameID++;
//    }
    MLogger::LOG(Level::DEBUG, "deviceAVPlayThread: -");
    //return ;
}

static PVOID deviceAudioThread(PVOID args) {
    STATUS status;
    UINT64 pts = 0;
    static Frame vFrame, aFrame;
    UINT64 runningTime;
    UINT32 fileIndex = 0;
    sManualConfig.start_apts = get_current_pts();
    sManualConfig.aFrame.version = FRAME_CURRENT_VERSION;
    sManualConfig.aFrame.trackId = DEFAULT_AUDIO_TRACK_ID;
    sManualConfig.aFrame.duration = 0;
    sManualConfig.aFrame.decodingTs = 0;     /*!< Relative time mode */
    sManualConfig.aFrame.presentationTs = 0; /*!< Relative time mode */
    sManualConfig.aFrame.index = 0;
    sManualConfig.aFrame.flags = FRAME_FLAG_NONE; /*!< audio track is not used to cut fragment */

    AMExportPacket aPacket;
    MLogger::LOG(Level::DEBUG, "KvsPlayer::putAFrameRoutine: +");
    while(true) {
        if (ComponentProvider::GetInstance()->GetStreamSource(ORYX)->GetAFrame(&aPacket) == 0) {
            ComponentProvider::GetInstance()->GetKvsRender(RenderType::AWSPRODUCER)
                ->KvsProducerPutFrameRoutine(&sManualConfig, &aPacket);
        } else {
            MLogger::LOG(Level::ERROR, "KvsPlayer::GetAFrame: ERROR");
        }
        ComponentProvider::GetInstance()->GetStreamSource(ORYX)->ReleaseAFrame(&aPacket);
    }
    MLogger::LOG(Level::DEBUG, "KvsPlayer::putAFrameRoutine: -");
    //#endif

    //MLogger::LOG(Level::DEBUG, "putAVFrameRoutine: +");

    //while (GETTIME() < streamStopTime) {
    //status = ComponentProvider::GetInstance()->GetKvsRender(RenderType::AWSPRODUCER)->PutVideoFrame(&frame);
    //if (!firstVideoFramePut) {
    //    startUpLatency = (DOUBLE) (GETTIME() - startTime) / (DOUBLE) HUNDREDS_OF_NANOS_IN_A_MILLISECOND;
    //    DLOGD("Start up latency: %lf ms", startUpLatency);
    //    firstVideoFramePut = true;
    //}
    //if (firstPTSSetted == false) {
    //    firstVideoPTS = pts;
    //    firstPTSSetted = true;
    //}
    //frame.presentationTs = (pts - firstVideoPTS) * 111;
    //frame.decodingTs = frame.presentationTs;
    //frame.index++;

    //frame.flags = frame.index % DEFAULT_KEY_FRAME_INTERVAL == 0 ? FRAME_FLAG_KEY_FRAME : FRAME_FLAG_NONE;;
    //ComponentProvider::GetInstance()->GetStreamSource(FAKE)->GetVideoFrame(&frame.frameData, &frame.size, &pts);

    //runningTime = GETTIME() - streamStartTime;
    //if (runningTime < frame.presentationTs) {
    //            THREAD_SLEEP((frame.presentationTs - runningTime) * 0.9);
    //        }
    //        frameID++;
    //    }
    MLogger::LOG(Level::DEBUG, "putAVFrameRoutine: -");
    return 0;
}

static PVOID deviceVideoThread(PVOID args) {
    STATUS status;
    UINT64 pts = 0;
    static Frame vFrame, aFrame;
    UINT64 runningTime;
    UINT32 fileIndex = 0;
    sManualConfig.start_vpts = get_current_pts();
    sManualConfig.vFrame.version = FRAME_CURRENT_VERSION;
    sManualConfig.vFrame.trackId = DEFAULT_VIDEO_TRACK_ID;
    sManualConfig.vFrame.duration = 0;
    sManualConfig.vFrame.presentationTs = 0;
    sManualConfig.vFrame.decodingTs = 0;
    sManualConfig.vFrame.index = 0;

    //#ifdef CV28_BUILD
    AMExportPacket vPacket;
    MLogger::LOG(Level::DEBUG, "KvsPlayer::putVFrameRoutine: +");
    while(true) {
        if (ComponentProvider::GetInstance()->GetStreamSource(ORYX)->GetVFrame(&vPacket) == 0) {
            ComponentProvider::GetInstance()->GetKvsRender(RenderType::AWSPRODUCER)
                ->KvsProducerPutFrameRoutine(&sManualConfig, &vPacket);
        } else {
            MLogger::LOG(Level::ERROR, "KvsPlayer::GetVFrame: ERROR");
        }
        ComponentProvider::GetInstance()->GetStreamSource(ORYX)->ReleaseVFrame(&vPacket);
    }
    MLogger::LOG(Level::DEBUG, "KvsPlayer::putVFrameRoutine: -");
    //#endif

    //MLogger::LOG(Level::DEBUG, "putAVFrameRoutine: +");

    //while (GETTIME() < streamStopTime) {
    //status = ComponentProvider::GetInstance()->GetKvsRender(RenderType::AWSPRODUCER)->PutVideoFrame(&frame);
    //if (!firstVideoFramePut) {
    //    startUpLatency = (DOUBLE) (GETTIME() - startTime) / (DOUBLE) HUNDREDS_OF_NANOS_IN_A_MILLISECOND;
    //    DLOGD("Start up latency: %lf ms", startUpLatency);
    //    firstVideoFramePut = true;
    //}
    //if (firstPTSSetted == false) {
    //    firstVideoPTS = pts;
    //    firstPTSSetted = true;
    //}
    //frame.presentationTs = (pts - firstVideoPTS) * 111;
    //frame.decodingTs = frame.presentationTs;
    //frame.index++;

    //frame.flags = frame.index % DEFAULT_KEY_FRAME_INTERVAL == 0 ? FRAME_FLAG_KEY_FRAME : FRAME_FLAG_NONE;;
    //ComponentProvider::GetInstance()->GetStreamSource(FAKE)->GetVideoFrame(&frame.frameData, &frame.size, &pts);

    //runningTime = GETTIME() - streamStartTime;
    //if (runningTime < frame.presentationTs) {
    //            THREAD_SLEEP((frame.presentationTs - runningTime) * 0.9);
    //        }
    //        frameID++;
    //    }
    MLogger::LOG(Level::DEBUG, "putAVFrameRoutine: -");
    return 0;
}

static PVOID PCAudioFrameThread(PVOID args) {
    STATUS status;
    UINT64 pts = 0;
    static Frame frame;
    PPCSampleCustomData data = (PPCSampleCustomData) args;
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
    while( GETTIME() < data->streamStopTime) {
#ifdef CONFIG_AUDIO_ONLY
        MLogger::LOG(Level::DEBUG, "putAudioFrameRoutine: firstVideoFramePut = true");
        firstVideoFramePut = true;
#endif
        if (ATOMIC_LOAD_BOOL(&data->firstVideoFramePut)) {
            ComponentProvider::GetInstance()->GetStreamSource(FAKE)->GetAudioFrame(&frame.frameData, &frame.size, &pts);
            status = ComponentProvider::GetInstance()->GetKvsRender(RenderType::AWSPRODUCER)->PutAudioFrame(&frame);
            if (STATUS_FAILED(status)) {
                //printf("putKinesisVideoFrame for audio failed with 0x%08x\n", status);
                MLogger::LOG(Level::ERROR, "putAudioFrameRoutine, (index=%d), status = 0x%08x", frame.index, status);
            } else {
                // MLogger::LOG(Level::DEBUG, "putAudioFrameRoutine, (index=%d), status = 0x%08x", frame.index, status);
            }

            frame.presentationTs += SAMPLE_AUDIO_FRAME_DURATION;
            frame.decodingTs = frame.presentationTs;
            frame.index++;

            // synchronize putKinesisVideoFrame to running time
            runningTime = GETTIME() - data->streamStartTime;
            if (runningTime < frame.presentationTs) {
                THREAD_SLEEP(frame.presentationTs - runningTime);
            }
        }

    }

    MLogger::LOG(Level::DEBUG, "putAudioFrameRoutine: -");
    return 0;
}

static PVOID PCVideoFrameThread(PVOID args) {
    STATUS status;
    UINT64 pts = 0;
    PPCSampleCustomData data = (PPCSampleCustomData) args;
    static Frame frame;
    UINT64 runningTime;
    UINT32 fileIndex = 0;
    double startUpLatency;
    frame.version = FRAME_CURRENT_VERSION;
    frame.trackId = DEFAULT_VIDEO_TRACK_ID;
    frame.duration = 0;
    frame.decodingTs = 0;
    frame.presentationTs = 0;
    frame.index = 0;
    frame.flags = FRAME_FLAG_KEY_FRAME;
    MLogger::LOG(Level::DEBUG, "PCVideoFrameThread: +");
    ComponentProvider::GetInstance()->GetStreamSource(FAKE)->GetVideoFrame(&frame.frameData, &frame.size, &pts);

    while (GETTIME() < data->streamStopTime) {
        status = ComponentProvider::GetInstance()->GetKvsRender(AWSPRODUCER)->PutVideoFrame(&frame);

        if (data->firstFrame) {
            //startUpLatency = (DOUBLE) (GETTIME() - data->startTime) / (DOUBLE) HUNDREDS_OF_NANOS_IN_A_MILLISECOND;
            //DLOGD("Start up latency: %lf ms", startUpLatency);
            data->firstFrame = false;
        } else if (frame.flags == FRAME_FLAG_KEY_FRAME) {
            // generate an image and notification event at the start of the video stream.
            //putKinesisVideoEventMetadata(streamHandle, STREAM_EVENT_TYPE_NOTIFICATION | STREAM_EVENT_TYPE_IMAGE_GENERATION, NULL);
            //MLogger::LOG(Level::DEBUG, "putVideoFrameRoutine: FirstKeyFrarme: ");
            // only push this once in this sample. A customer may use this whenever it is necessary though
        }
        ATOMIC_STORE_BOOL(&data->firstVideoFramePut, TRUE);

        if (STATUS_SUCCEEDED(status)) {
            //MLogger::LOG(Level::DEBUG, "putKinesisVideoFrame, (index=%d), status = 0x%08x", frameID, status);
        } else {
            MLogger::LOG(Level::ERROR, "putKinesisVideoFrame, (index=%d), status = 0x%08x", frame.index, status);
        }

        frame.presentationTs += SAMPLE_VIDEO_FRAME_DURATION;
        frame.decodingTs = frame.presentationTs;
        frame.index++;

        fileIndex = (fileIndex + 1) % NUMBER_OF_VIDEO_FRAME_FILES;
        frame.flags = fileIndex % DEFAULT_KEY_FRAME_INTERVAL == 0 ? FRAME_FLAG_KEY_FRAME : FRAME_FLAG_NONE;
        ComponentProvider::GetInstance()->GetStreamSource(FAKE)->GetVideoFrame(&frame.frameData, &frame.size, &pts);

        runningTime = GETTIME() - data->streamStartTime;
        if (runningTime < frame.presentationTs) {
            // reduce sleep time a little for smoother video
            THREAD_SLEEP((frame.presentationTs - runningTime) * 0.9);
        }
    }
    MLogger::LOG(Level::DEBUG, "PCVideoFrameThread: -");
    return 0;
}

static PVOID PCAVFrameThread(PVOID args) {
    STATUS status;
    UINT64 pts = 0;
    PPCSampleCustomData data = (PPCSampleCustomData) args;
    static Frame aframe;
    static Frame vframe;
    UINT64 runningTime;
    UINT32 fileIndex = 0;

    MLogger::LOG(Level::DEBUG, "PCAVFrameThread: +");
    while (GETTIME() < data->streamStopTime) {
        ComponentProvider::GetInstance()->GetStreamSource(FAKE)->GetVideoFrame(&vframe.frameData, &vframe.size, &pts);
        ComponentProvider::GetInstance()->GetStreamSource(FAKE)->GetAudioFrame(&aframe.frameData, &aframe.size, &pts);
        if (data->firstFrame) {
            // Init vFrame
            vframe.version = FRAME_CURRENT_VERSION;
            vframe.trackId = DEFAULT_VIDEO_TRACK_ID;
            vframe.duration = 0;
            vframe.decodingTs = 0;
            vframe.presentationTs = 0;
            vframe.index = 0;
            vframe.flags = FRAME_FLAG_KEY_FRAME;

            // Init aFrame
            aframe.version = FRAME_CURRENT_VERSION;
            aframe.trackId = DEFAULT_AUDIO_TRACK_ID;
            aframe.duration = 0;
            aframe.decodingTs = 0;
            aframe.presentationTs = 0;
            aframe.index = 0;

            data->firstFrame = false;
        } else {
            vframe.presentationTs += SAMPLE_VIDEO_FRAME_DURATION;
            vframe.decodingTs = vframe.presentationTs;
            vframe.index++;
            fileIndex = (fileIndex + 1) % NUMBER_OF_VIDEO_FRAME_FILES;
            vframe.flags = fileIndex % DEFAULT_KEY_FRAME_INTERVAL == 0 ? FRAME_FLAG_KEY_FRAME : FRAME_FLAG_NONE;

            aframe.presentationTs += SAMPLE_VIDEO_FRAME_DURATION;
            aframe.decodingTs = aframe.presentationTs;
            aframe.index++;
        }
        status = ComponentProvider::GetInstance()->GetKvsRender(AWSPRODUCER)->PutVideoFrame(&vframe);
        //status = putKinesisVideoFrame(*sPStreamHandle, &vframe);
        if (!STATUS_SUCCEEDED(status)) {
            MLogger::LOG(Level::DEBUG, "putKinesisVideoFrame: Video: status=0x%08x", status);
        } else {
            MLogger::LOG(Level::DEBUG, "putKinesisVideoFrame: Video: status=0x%08x", status);
        }
        ATOMIC_STORE_BOOL(&data->firstVideoFramePut, TRUE);

        //status = putKinesisVideoFrame(*sPStreamHandle, &aframe);
        status = ComponentProvider::GetInstance()->GetKvsRender(AWSPRODUCER)->PutAudioFrame(&aframe);
        if (!STATUS_SUCCEEDED(status)) {
            MLogger::LOG(Level::DEBUG, "putKinesisVideoFrame: Audio: status=0x%08x", status);
        } else {
            MLogger::LOG(Level::DEBUG, "putKinesisVideoFrame: Audio: status=0x%08x", status);
        }

        // synchronize putKinesisVideoFrame to running time
        runningTime = GETTIME() - data->streamStartTime;
        if (runningTime < vframe.presentationTs) {
            // reduce sleep time a little for smoother video
            THREAD_SLEEP((vframe.presentationTs - runningTime) * 0.9);
        }

    }
    MLogger::LOG(Level::DEBUG, "PCAVFrameThread: -");
    return 0;
}

static TID audioSendTid, videoSendTid, avSendTid, aSendTid, vSendTid;
int KvsPlayer::HandleAsyncMethod(const MethodItem& method) {
    UINT64 streamStopTime = GETTIME() + DEFAULT_STREAM_DURATION;

    ComponentProvider::GetInstance()->GetKvsRender(RenderType::AWSPRODUCER)->BaseInit();

    if (method.m_data.find("pc") !=std::string::npos ) {
        MEMSET(&sPCSampleCustomData, 0x00, SIZEOF(PCSampleCustomData));
        ATOMIC_STORE_BOOL(&sPCSampleCustomData.firstVideoFramePut, FALSE);
        sPCSampleCustomData.startTime = GETTIME();
        sPCSampleCustomData.firstFrame = TRUE;
        sPCSampleCustomData.streamStopTime =  streamStopTime;
        sPCSampleCustomData.streamStartTime = GETTIME();
        ATOMIC_STORE_BOOL(&sPCSampleCustomData.firstVideoFramePut, FALSE);
    }

    if ("Start" == method.m_method && "pc-v" == method.m_data) {
        THREAD_CREATE(&vSendTid, PCVideoFrameThread, &sPCSampleCustomData);
        THREAD_JOIN(vSendTid, nullptr);
    } else if ("Start" == method.m_method && "pc-av" == method.m_data) {
        switch (sAVCONFIG) {
            case AV_CONFIG::IN_A_THREAD:
                THREAD_CREATE(&avSendTid, PCAVFrameThread, &sPCSampleCustomData);
                THREAD_JOIN(avSendTid, nullptr);
                break;
            case AV_CONFIG::IN_TWO_THREADS:
                THREAD_CREATE(&vSendTid, PCVideoFrameThread, &sPCSampleCustomData);
                THREAD_CREATE(&aSendTid, PCAudioFrameThread, &sPCSampleCustomData);
                THREAD_JOIN(vSendTid, nullptr);
                THREAD_JOIN(aSendTid, nullptr);
                break;
            default:
                break;
        }
    } else if ("Start" == method.m_method) {
        // ToDo: Parse parameter to choose one item among
        // AV concurrency
        switch (sAVCONFIG) {
            case AV_CONFIG::IN_A_THREAD:
                THREAD_CREATE(&avSendTid, deviceAVPlayThread, &sPCSampleCustomData);
                THREAD_JOIN(avSendTid, nullptr);
                break;
            case AV_CONFIG::IN_TWO_THREADS:
                THREAD_CREATE(&vSendTid, deviceVideoThread, &sPCSampleCustomData);
                THREAD_CREATE(&aSendTid, deviceAudioThread, &sPCSampleCustomData);
                THREAD_JOIN(vSendTid, nullptr);
                THREAD_JOIN(aSendTid, nullptr);
                break;
            default:
                break;
        }

        // Audio Only

        // Video Only
    }

    return 0;
}

KvsPlayer::KvsPlayer(): ServiceBase("KvsPlayer") {}
#define HW_TIMER ((const char *)"/proc/ambarella/ambarella_hwtimer")
KvsPlayer::~KvsPlayer() {}

int KvsPlayer::Init() {
    g_hw_timer_fd = open(HW_TIMER, O_RDONLY);
    if (g_hw_timer_fd < 0)
    {
        printf("[ERROR] Failed to open hardware timer\n");

    }
    ServiceBaseInit();
    return 0;
}

int KvsPlayer::Deinit()
{
    ServiceBaseDeinit();
    return 0;
}

int KvsPlayer::Start(std::string option) {
    MLogger::LOG(Level::DEBUG, "KvsPlayer::Start: option=%s", option.c_str());
    EnqueuenMethod(MethodItem("Start", std::string(option)));
    return 0;
}

int KvsPlayer::Pause() {
    return 0;
}

int KvsPlayer::Stop() {
    ComponentProvider::GetInstance()->GetKvsRender(RenderType::AWSPRODUCER)->BaseDeinit();
    return 0;
}
//======================================================
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

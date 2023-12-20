#include "KvsProducer.h"
#include "AllInterfaces.h"
#include "ComponentProvider.h"
#include "KvsServiceConfig.h"
#include "MLogger.h"
#include "chrono"

static PDeviceInfo mpDeviceInfo;
static PStreamInfo mpStreamInfo = NULL;
static PTrackInfo mpAudioTrack = NULL;
static PCHAR mStreamName;
static PClientCallbacks mpClientCallbacks = NULL;
static PStreamCallbacks mpStreamCallbacks = NULL;
static CLIENT_HANDLE mClientHandle = INVALID_CLIENT_HANDLE_VALUE;
PTrackInfo pAudioTrack = NULL;
static PDeviceInfo sPDeviceInfo;
#define DEFAULT_STORAGE_SIZE              20 * 1024 * 1024
#define RECORDED_FRAME_AVG_BITRATE_BIT_PS 3800000
static STREAM_HANDLE mStreamHandle = INVALID_STREAM_HANDLE_VALUE;
static PSTREAM_HANDLE sPStreamHandle = &mStreamHandle;
static PStreamInfo pStreamInfo = NULL;
static PTrackInfo spAudioTrack;
static BYTE sAACAudioCpd[KVS_AAC_CPD_SIZE_BYTE];
static PClientCallbacks pClientCallbacks = NULL;
static PStreamCallbacks pStreamCallbacks = NULL;
static CLIENT_HANDLE clientHandle = INVALID_CLIENT_HANDLE_VALUE;
BYTE aacAudioCpd[KVS_AAC_CPD_SIZE_BYTE];

static UINT64 gStartTime;
static UINT64 gStreamStartTime;
static UINT64 gFirstVideoPTS;
static UINT64 gFirstAudioPTS;
static UINT64 gStreamStopTime;
//static BOOL gIsFirstFrameSent;
//static PSEMAPHORE_HANDLE mpStreamHandle;

static BYTE mAACAudioCpd[KVS_AAC_CPD_SIZE_BYTE];
static UINT8 gEventsEnabled = 0;
volatile ATOMIC_BOOL gIsFirstVideoFramePut = false;
static SampleStreamSource* psStreamSource;

void KvsProducer::KvsProducerPutFrameRoutine(MediaStreamConfig *streamConfig, AMExportPacket *packet)
{
    STATUS status;

    switch (packet->packet_type)
    {
        case AM_EXPORT_PACKET_TYPE_INVALID:
        {
            printf("KvsProducerPutFrameRoutine: AM_EXPORT_PACKET_TYPE_INVALID\n");
            printf("received invalid packet type\n");
        }
        break;
        case AM_EXPORT_PACKET_TYPE_VIDEO_DATA:
        {
            //printf("KvsProducerPutFrameRoutine: AM_EXPORT_PACKET_TYPE_VIDEO_DATA\n");
            streamConfig->video_sent = true;
            if (streamConfig->video_first_packet_flag == false)
            {
                streamConfig->video_first_packet_flag = true;
                streamConfig->first_video_pts = packet->pts;
            }
            else
            {
                streamConfig->vFrame.presentationTs = ((packet->pts - streamConfig->first_video_pts) * 111);
                streamConfig->vFrame.decodingTs = streamConfig->vFrame.presentationTs;
                streamConfig->vFrame.index++;
            }

            streamConfig->vFrame.flags = ((packet->frame_type == AM_VIDEO_FRAME_TYPE_IDR) ||
                                          (packet->frame_type == AM_VIDEO_FRAME_TYPE_I))
                ? FRAME_FLAG_KEY_FRAME
                : FRAME_FLAG_NONE;
            if (FRAME_FLAG_KEY_FRAME == streamConfig->vFrame.flags) {
                printf("AM_EXPORT_PACKET_TYPE_VIDEO_DATA::   FRAME_FLAG_KEY_FRAME\n");
            }
            streamConfig->vFrame.frameData = packet->data_ptr;
            streamConfig->vFrame.size = packet->data_size;

            if (*sPStreamHandle == INVALID_STREAM_HANDLE_VALUE)
            {
                printf("stream_handle is invalid in put video fram\n");
                return;
            }

            status = putKinesisVideoFrame(*sPStreamHandle, &streamConfig->vFrame);
            if (STATUS_FAILED(status))
            {
                printf("putKinesisVideoFrame: AM_EXPORT_PACKET_TYPE_VIDEO_DATA failed with 0x%08x\n", status);
            }

            // TODO: allow mkv tags to be correctly passed (workround solution)
            /*
            if (!streamConfig->start_tag_flag)
            {
                char meta_type[] = "VIDEO_INTERVAL_START";
                streamConfig->start_tag_flag = send_kvs_metadata(streamConfig, meta_type);
            }*/
        }
        break;
        case AM_EXPORT_PACKET_TYPE_AUDIO_DATA:
        {
            if (!streamConfig->video_sent) {
                printf("KvsProducerPutFrameRoutine: AM_EXPORT_PACKET_TYPE_AUDIO_DATA: wait video\n");
                break;
            } else {
                //printf("KvsProducerPutFrameRoutine: AM_EXPORT_PACKET_TYPE_AUDIO_DATA\n");
            }
            if (streamConfig->audio_first_packet_flag == false)
            {
                streamConfig->audio_first_packet_flag = true;
                streamConfig->first_audio_pts = packet->pts;
            }
            else
            {
                streamConfig->aFrame.presentationTs = ((packet->pts - streamConfig->first_audio_pts) * 111);
                streamConfig->aFrame.decodingTs = streamConfig->aFrame.presentationTs;
                streamConfig->aFrame.index++;
            }

            streamConfig->aFrame.flags = FRAME_FLAG_NONE;
            streamConfig->aFrame.frameData = packet->data_ptr + 7;
            streamConfig->aFrame.size = packet->data_size - 7;
            // printf("acurr pts:%ld audio pts:%ld ,keyFrame:%d ,size:%d\n",get_current_pts() ,aFrame.presentationTs/100,aFrame.flags,aFrame.size);
            if (*sPStreamHandle == INVALID_STREAM_HANDLE_VALUE)
            {
                printf("stream_handle is invalid in put auido frame\n");
                return ;
            }

            status = putKinesisVideoFrame(*sPStreamHandle, &streamConfig->aFrame);
            if (STATUS_FAILED(status))
            {
                printf("putKinesisVideoFrame: AM_EXPORT_PACKET_TYPE_AUDIO_DATA failed with 0x%08x\n", status);
            }
        }
        break;
        default:
        {
            printf("KvsProducerPutFrameRoutine: default\n");
            printf("discard non-video-audio packet here\n");
        }
        break;
    }

    return;
}


PVOID putVideoFrameRoutine(PVOID args)
{
    /*
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

    // video track is used to mark new fragment. A new fragment is generated for every frame with FRAME_FLAG_KEY_FRAME
    frame.flags = fileIndex % DEFAULT_KEY_FRAME_INTERVAL == 0 ? FRAME_FLAG_KEY_FRAME : FRAME_FLAG_NONE;

    while (GETTIME() < gStreamStopTime) {
        status = putKinesisVideoFrame(psStreamSource->streamHandle, &frame);
        if (gIsFirstVideoFramePut) {
            //startUpLatency = (DOUBLE) (GETTIME() - gStartTime) / (DOUBLE) HUNDREDS_OF_NANOS_IN_A_MILLISECOND;
            //DLOGD("Start up latency: %lf ms", startUpLatency);
            //gFirstVideoPTS
            gIsFirstVideoFramePut = true;
        } else if (frame.flags == FRAME_FLAG_KEY_FRAME && gEventsEnabled) {
            // generate an image and notification event at the start of the video stream.
            //putKinesisVideoEventMetadata(data->streamHandle, STREAM_EVENT_TYPE_NOTIFICATION | STREAM_EVENT_TYPE_IMAGE_GENERATION, NULL);
            putKinesisVideoEventMetadata(psStreamSource->streamHandle, STREAM_EVENT_TYPE_NOTIFICATION | STREAM_EVENT_TYPE_IMAGE_GENERATION, NULL);
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
        int64_t pts;
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
     */
    return 0;
}

PVOID putAudioFrameRoutine(PVOID args)
{
    /*
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
            status = putKinesisVideoFrame(psStreamSource->streamHandle, &frame);
            if (STATUS_FAILED(status)) {
                printf("putKinesisVideoFrame for audio failed with 0x%08x\n", status);
                status = STATUS_SUCCESS;
            }

            frame.presentationTs += SAMPLE_AUDIO_FRAME_DURATION;
            frame.decodingTs = frame.presentationTs;
            frame.index++;

            fileIndex = (fileIndex + 1) % NUMBER_OF_AUDIO_FRAME_FILES;
            int64_t pts;
            ComponentProvider::GetInstance()->GetStreamSource(FAKE)->GetAudioFrame(&frame.frameData, &frame.size, &pts);

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
     */
    return 0;
}
TID audioSendTid, videoSendTid;

KvsProducer::KvsProducer() {}

KvsProducer::~KvsProducer() {}

int KvsProducer::SetDataSource(SampleStreamSource* psource) {
    psStreamSource = psource;
    return 0;
}

int KvsProducer::StartUpload() {
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

//int KvsProducer::SetHandler(STREAM_HANDLE* handler) {
//  //  mpStreamHandle = handler;
//    return 0;
//}

int KvsProducer::BaseInit() {
    return Init();
}

int KvsProducer::BaseDeinit() {
    return Deinit();
}


STATUS StreamCB_StreamUnderflowReportFunc(UINT64, STREAM_HANDLE) {
    MLogger::LOG(Level::DEBUG, "StreamCB-onStreamUnderflowReportFunc");
    return STATUS_SUCCESS;
}

STATUS StreamCB_BufferDurationOverflowPressureFunc(UINT64, STREAM_HANDLE, UINT64) {
    MLogger::LOG(Level::DEBUG, "StreamCB-onBufferDurationOverflowPressureFunc");
    return STATUS_SUCCESS;
}

STATUS StreamCB_StreamLatencyPressureFunc(UINT64, STREAM_HANDLE, UINT64) {
    MLogger::LOG(Level::DEBUG, "StreamCB-onStreamLatencyPressureFunc");
    return STATUS_SUCCESS;
}

STATUS StreamCB_StreamConnectionStaleFunc(UINT64, STREAM_HANDLE, UINT64) {
    MLogger::LOG(Level::DEBUG, "StreamCB-onStreamConnectionStaleFunc");
    /*
    static bool sent = false;
    if (!sent) {
        StreamEventMetadata Meta{STREAM_EVENT_METADATA_CURRENT_VERSION, NULL, 1, {}, {}};
        CHAR tagName1[10] = {'\0'};
        CHAR tagValue1[10] = {'\0'};
        Meta.names[0] = tagName1;
        Meta.values[0] = tagValue1;
        MEMCPY(tagName1, (PCHAR) "TYPE", STRLEN("tagName"));
        MEMCPY(tagValue1, (PCHAR) "Value", STRLEN("tagValue"));
        STATUS s2 = putKinesisVideoEventMetadata(*sPStreamHandle,
                                                 STREAM_EVENT_TYPE_NOTIFICATION | STREAM_EVENT_TYPE_IMAGE_GENERATION,
                                                 &Meta);
        MLogger::LOG(Level::DEBUG, "onStreamConnectionStaleFunc: putKinesisVideoEventMetadata: result=0x%08x", s2);
        sent = true;
    }*/
    return STATUS_SUCCESS;
}
STATUS StreamCB_DroppedFrameReportFunc(UINT64, STREAM_HANDLE, UINT64) {
    //MLogger::LOG(Level::DEBUG, "StreamCB-onDroppedFrameReportFunc");
    return STATUS_SUCCESS;
}
STATUS StreamCB_DroppedFragmentReportFunc(UINT64, STREAM_HANDLE, UINT64) {
    MLogger::LOG(Level::DEBUG, "StreamCB-onDroppedFragmentReportFunc");
    return STATUS_SUCCESS;
}
STATUS StreamCB_StreamErrorReportFunc(UINT64, STREAM_HANDLE, UPLOAD_HANDLE, UINT64, STATUS status) {
    MLogger::LOG(Level::DEBUG, "StreamCB-onStreamErrorReportFunc: STATUS=0x%08x", status);
    return STATUS_SUCCESS;
}

STATUS StreamCB_FragmentAckReceivedFunc(UINT64, STREAM_HANDLE, UPLOAD_HANDLE, PFragmentAck) {
    MLogger::LOG(Level::DEBUG, "StreamCB-onFragmentAckReceivedFunc");
    static bool sent = true;
    if (!sent) {
        StreamEventMetadata Meta{STREAM_EVENT_METADATA_CURRENT_VERSION, NULL, 1, {}, {}};
        CHAR tagName1[10] = {'\0'};
        CHAR tagValue1[10] = {'\0'};
        Meta.names[0] = tagName1;
        Meta.values[0] = tagValue1;
        MEMCPY(tagName1, (PCHAR) "TYPE", STRLEN("tagName"));
        MEMCPY(tagValue1, (PCHAR) "Value", STRLEN("tagValue"));
        STATUS s2 = putKinesisVideoEventMetadata(*sPStreamHandle,
                                                 STREAM_EVENT_TYPE_NOTIFICATION | STREAM_EVENT_TYPE_IMAGE_GENERATION,
                                                 &Meta);
        MLogger::LOG(Level::DEBUG, "onFragmentAckReceivedFunc: putKinesisVideoEventMetadata: result=0x%08x", s2);
        sent = true;
    }
    return STATUS_SUCCESS;
}

STATUS StreamCB_StreamDataAvailableFunc(UINT64, STREAM_HANDLE, PCHAR, UPLOAD_HANDLE, UINT64, UINT64) {
    //MLogger::LOG(Level::DEBUG, "StreamCB-onStreamDataAvailableFunc");
    /*
    static bool sent = false;
    if (!sent) {
        StreamEventMetadata Meta{STREAM_EVENT_METADATA_CURRENT_VERSION, NULL, 1, {}, {}};
        CHAR tagName1[10] = {'\0'};
        CHAR tagValue1[10] = {'\0'};
        Meta.names[0] = tagName1;
        Meta.values[0] = tagValue1;
        MEMCPY(tagName1, (PCHAR) "TYPE", STRLEN("tagName"));
        MEMCPY(tagValue1, (PCHAR) "Value", STRLEN("tagValue"));
        STATUS s2 = putKinesisVideoEventMetadata(*sPStreamHandle,
                                                 STREAM_EVENT_TYPE_NOTIFICATION | STREAM_EVENT_TYPE_IMAGE_GENERATION,
                                                 &Meta);
        MLogger::LOG(Level::DEBUG, "onStreamDataAvailableFunc: putKinesisVideoEventMetadata: result=0x%08x", s2);
        sent = true;
    }*/
    return STATUS_SUCCESS;
}

STATUS StreamCB_StreamReadyFunc(UINT64, STREAM_HANDLE) {
    MLogger::LOG(Level::DEBUG, "StreamCB-onStreamReadyFunc");
    /*
    static bool sent = false;
    if (!sent) {
        StreamEventMetadata Meta{STREAM_EVENT_METADATA_CURRENT_VERSION, NULL, 1, {}, {}};
        CHAR tagName1[10] = {'\0'};
        CHAR tagValue1[10] = {'\0'};
        Meta.names[0] = tagName1;
        Meta.values[0] = tagValue1;
        MEMCPY(tagName1, (PCHAR) "TYPE", STRLEN("tagName"));
        MEMCPY(tagValue1, (PCHAR) "Value", STRLEN("tagValue"));
        STATUS s2 = putKinesisVideoEventMetadata(*sPStreamHandle,
                                                 STREAM_EVENT_TYPE_NOTIFICATION | STREAM_EVENT_TYPE_IMAGE_GENERATION,
                                                 &Meta);
        MLogger::LOG(Level::DEBUG, "onStreamReadyFunc: putKinesisVideoEventMetadata: result=0x%08x", s2);
        sent = true;
    }*/
    return STATUS_SUCCESS;
}

STATUS StreamCB_StreamClosedFunc(UINT64, STREAM_HANDLE, UPLOAD_HANDLE) {
    MLogger::LOG(Level::DEBUG, "StreamCB-onStreamClosedFunc");
    return STATUS_SUCCESS;
}

STATUS StreamCB_StreamShutdownFunc(UINT64, STREAM_HANDLE, BOOL) {
    MLogger::LOG(Level::DEBUG, "StreamCB-onStreamShutdownFunc");
    return STATUS_SUCCESS;
}

STATUS StreamCB_FreeStreamCallbacksFunc(PUINT64) {
    MLogger::LOG(Level::DEBUG, "StreamCB-onFreeStreamCallbacksFunc");
    return STATUS_SUCCESS;
}

int KvsProducer::Init2() {
    mSettings.Init();
    // Step: 0
    //auto start = std::chrono::high_resolution_clock::now();
    mSettings.GetString(EVENT_STREAM_NAME, mStreamName);
    mSettings.GetString(END_POINT, mIotCoreCredentialEndPoint);
    mSettings.GetString(CERT_LOCATION, mIotCoreCert);
    mSettings.GetString(KEY_LOCATION, mIotCorePrivateKey);
    mSettings.GetString(CA_LOCATION, mCaCert);
    mSettings.GetString(ROLE_ALIAS, mIotCoreRoleAlias);
    mSettings.GetString(THING_NAME, mThingName);
    mSettings.GetString(REGION, mRegion);

    STATUS status;

    // Step-0
    status = createDefaultDeviceInfo(&sPDeviceInfo);
    MLogger::LOG(Level::DEBUG, "Init: createDefaultDeviceInfo: %x", status);
    sPDeviceInfo->clientInfo.loggerLogLevel = LOG_LEVEL_ERROR;
    sPDeviceInfo->storageInfo.storageSize = DEFAULT_STORAGE_SIZE;

    // Step 1: HERE HERE
    MLogger::LOG(Level::DEBUG, "Init: createRealtimeAudioVideoStreamInfoProviderWithCodecs");
// Audio-Only (Failed)
//    status = createRealtimeAudioStreamInfoProviderWithCodecs((PCHAR )mStreamName.data(), DEFAULT_RETENTION_PERIOD, DEFAULT_BUFFER_DURATION, AUDIO_CODEC_ID_AAC,
//                                                                    &pStreamInfo);
// Video-Only
//    status = createRealtimeVideoStreamInfoProviderWithCodecs((PCHAR )mStreamName.data(), DEFAULT_RETENTION_PERIOD, DEFAULT_BUFFER_DURATION, VIDEO_CODEC_ID_H264,
//                                                                  &pStreamInfo);
// Video-And-Audio
    status = createRealtimeAudioVideoStreamInfoProviderWithCodecs((PCHAR )mStreamName.data(), DEFAULT_RETENTION_PERIOD, DEFAULT_BUFFER_DURATION, VIDEO_CODEC_ID_H264,
                                                                  AUDIO_CODEC_ID_AAC, &pStreamInfo);
    MLogger::LOG(Level::DEBUG, "Init: createDefaultCallbacksProviderWithIotCertificate: %X", status);

    // set up audio cpd.
    pAudioTrack = pStreamInfo->streamCaps.trackInfoList[0].trackId == DEFAULT_AUDIO_TRACK_ID ? &pStreamInfo->streamCaps.trackInfoList[0]
                                                                                             : &pStreamInfo->streamCaps.trackInfoList[1];

    pAudioTrack->codecPrivateData = aacAudioCpd;
    pAudioTrack->codecPrivateDataSize = KVS_AAC_CPD_SIZE_BYTE;
    status = mkvgenGenerateAacCpd(AAC_LC, AAC_AUDIO_TRACK_SAMPLING_RATE, AAC_AUDIO_TRACK_CHANNEL_CONFIG, pAudioTrack->codecPrivateData,
                                  pAudioTrack->codecPrivateDataSize);
    MLogger::LOG(Level::DEBUG, "Init: mkvgenGenerateAacCpd: %x", status);

    pStreamInfo->streamCaps.absoluteFragmentTimes = FALSE;
    //pStreamInfo->streamCaps.frameOrderingMode = FRAME_ORDER_MODE_PASS_THROUGH;
    //pStreamInfo->streamCaps.frameOrderingMode = FRAME_ORDERING_MODE_MULTI_TRACK_AV_COMPARE_PTS_ONE_MS_COMPENSATE_EOFR;
    //start = std::chrono::high_resolution_clock::now();
    status = createDefaultCallbacksProviderWithIotCertificate((PCHAR )mIotCoreCredentialEndPoint.data(), (PCHAR )mIotCoreCert.data(), (PCHAR )mIotCorePrivateKey.data(),
                                                              (PCHAR )mCaCert.data(), (PCHAR )mIotCoreRoleAlias.data(), (PCHAR )mThingName.data(), (PCHAR )mRegion.data(),
                                                              NULL, NULL, &pClientCallbacks);
    MLogger::LOG(Level::DEBUG, "Init: createDefaultCallbacksProviderWithIotCertificate: %x", status);

    // step 4: addFileLoggerPlatformCallbacksProvider();
    STATUS retStatus = STATUS_SUCCESS;
    if (NULL != getenv(ENABLE_FILE_LOGGING)) {
        if ((retStatus = addFileLoggerPlatformCallbacksProvider(pClientCallbacks, FILE_LOGGING_BUFFER_SIZE, MAX_NUMBER_OF_LOG_FILES,
                                                                (PCHAR) "/data/tmp/middleware/", TRUE) != STATUS_SUCCESS)) {
            printf("File logging enable option failed with 0x%08x error code\n", retStatus);
        }
    }

    // step 5: createStreamCallbacks();
    //start = std::chrono::high_resolution_clock::now();
    status = createStreamCallbacks(&pStreamCallbacks);

    // Specialized cleanup callback
    FreeStreamCallbacksFunc freeStreamCallbacksFn;
    MLogger::LOG(Level::DEBUG, "Init: Create Stream Callbacks: %x", status);

    // step 6: addStreamCallbacks();
    status = addStreamCallbacks(pClientCallbacks, pStreamCallbacks);
    MLogger::LOG(Level::DEBUG, "Init: Add Stream Callbacks: %x", status);

    // step 7: createKinesisVideoClient();
    status = createKinesisVideoClient(sPDeviceInfo, pClientCallbacks, &clientHandle); // ToDo: Implement the callback functions
    MLogger::LOG(Level::DEBUG, "Init: 7. Create Kinesis Video Client: status = %x", status);

    // step 8: createKinesisVideoStreamSync();
    status = createKinesisVideoStreamSync(clientHandle, pStreamInfo, sPStreamHandle);
    MLogger::LOG(Level::DEBUG, "Init: 8. createKinesisVideoStreamSync: status = %x", status);

    return 0;
}

int KvsProducer::Init() {
    mSettings.Init();
    // Step: 0
    auto start = std::chrono::high_resolution_clock::now();
    mSettings.GetString(EVENT_STREAM_NAME, mStreamName);
    mSettings.GetString(END_POINT, mIotCoreCredentialEndPoint);
    mSettings.GetString(CERT_LOCATION, mIotCoreCert);
    mSettings.GetString(KEY_LOCATION, mIotCorePrivateKey);
    mSettings.GetString(CA_LOCATION, mCaCert);
    mSettings.GetString(ROLE_ALIAS, mIotCoreRoleAlias);
    mSettings.GetString(THING_NAME, mThingName);
    mSettings.GetString(REGION, mRegion);

    STATUS status;
    status = createDefaultDeviceInfo(&sPDeviceInfo);
    MLogger::LOG(Level::DEBUG, "Init: createDefaultDeviceInfo: %x", status);
    sPDeviceInfo->clientInfo.loggerLogLevel = LOG_LEVEL_ERROR;
    sPDeviceInfo->storageInfo.storageSize = DEFAULT_STORAGE_SIZE;

    // Step 1: HERE HERE
    //start = std::chrono::high_resolution_clock::now();
    // Audio-Only : status = createRealtimeAudioStreamInfoProviderWithCodecs((PCHAR )mStreamName.data(), DEFAULT_RETENTION_PERIOD, DEFAULT_BUFFER_DURATION, AUDIO_CODEC_ID_AAC, &pStreamInfo);

#ifdef CONFIG_VIDEO_AUDIO_BOTH
    MLogger::LOG(Level::DEBUG, "Init: createRealtimeAudioVideoStreamInfoProviderWithCodecs");
    //status = createOfflineVideoStreamInfoProviderWithCodecs((PCHAR )mStreamName.data(), DEFAULT_RETENTION_PERIOD, DEFAULT_BUFFER_DURATION, VIDEO_CODEC_ID_H264, &pStreamInfo);
    status = createRealtimeAudioVideoStreamInfoProviderWithCodecs((PCHAR )mStreamName.data(), DEFAULT_RETENTION_PERIOD, DEFAULT_BUFFER_DURATION, VIDEO_CODEC_ID_H264,
                                                  AUDIO_CODEC_ID_AAC, &pStreamInfo);
#endif

#ifdef CONFIG_AUDIO_ONLY
    MLogger::LOG(Level::DEBUG, "Init: createRealtimeAudioStreamInfoProviderWithCodecs");
    status = createRealtimeAudioStreamInfoProviderWithCodecs((PCHAR )mStreamName.data(), DEFAULT_RETENTION_PERIOD, DEFAULT_BUFFER_DURATION, AUDIO_CODEC_ID_AAC, &pStreamInfo);
#endif

#ifdef CONFIG_VIDEO_ONLY
    MLogger::LOG(Level::DEBUG, "Init: createRealtimeVideoStreamInfoProviderWithCodecs");
    // Failed to user Realtime video stream infor for PC version
    //status = createRealtimeVideoStreamInfoProviderWithCodecs((PCHAR )mStreamName.data(), DEFAULT_RETENTION_PERIOD, DEFAULT_BUFFER_DURATION, VIDEO_CODEC_ID_H264, &pStreamInfo);

    status = createOfflineVideoStreamInfoProviderWithCodecs((PCHAR )mStreamName.data(), DEFAULT_RETENTION_PERIOD, DEFAULT_BUFFER_DURATION, VIDEO_CODEC_ID_H264, &pStreamInfo);
#endif

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    MLogger::LOG(Level::DEBUG, "Init: createDefaultCallbacksProviderWithIotCertificate: %X, (duration=%d)", status, duration);

#ifndef CONFIG_VIDEO_ONLY
    // set up audio cpd.
    pAudioTrack = pStreamInfo->streamCaps.trackInfoList[0].trackId == DEFAULT_AUDIO_TRACK_ID ? &pStreamInfo->streamCaps.trackInfoList[0]
                                                                                            : &pStreamInfo->streamCaps.trackInfoList[1];

    pAudioTrack->codecPrivateData = aacAudioCpd;
    pAudioTrack->codecPrivateDataSize = KVS_AAC_CPD_SIZE_BYTE;
    status = mkvgenGenerateAacCpd(AAC_LC, AAC_AUDIO_TRACK_SAMPLING_RATE, AAC_AUDIO_TRACK_CHANNEL_CONFIG, pAudioTrack->codecPrivateData,
                                    pAudioTrack->codecPrivateDataSize);
    MLogger::LOG(Level::DEBUG, "Init: mkvgenGenerateAacCpd: %X", status);
#endif

    start = std::chrono::high_resolution_clock::now();
    status = setStreamInfoBasedOnStorageSize(DEFAULT_STORAGE_SIZE, RECORDED_FRAME_AVG_BITRATE_BIT_PS, 1, pStreamInfo);

    //start = std::chrono::high_resolution_clock::now();
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    MLogger::LOG(Level::DEBUG, "Init: setStreamInfoBasedOnStorageSize: %X, (duration=%d)", status, duration);

    pStreamInfo->streamCaps.absoluteFragmentTimes = FALSE;
    //pStreamInfo->streamCaps.frameTimecodes = FALSE;
    //pStreamInfo->streamCaps.frameOrderingMode = FRAME_ORDER_MODE_PASS_THROUGH;
    //FRAME_ORDERING_MODE_MULTI_TRACK_AV_COMPARE_PTS_ONE_MS_COMPENSATE_EOFR;
    //FRAME_ORDERING_MODE_MULTI_TRACK_AV_COMPARE_PTS_ONE_MS_COMPENSATE_EOFR;
    // createDefaultCallbacksProviderWithIotCertificate
    start = std::chrono::high_resolution_clock::now();
    status = createDefaultCallbacksProviderWithIotCertificate((PCHAR )mIotCoreCredentialEndPoint.data(), (PCHAR )mIotCoreCert.data(), (PCHAR )mIotCorePrivateKey.data(),
                                                              (PCHAR )mCaCert.data(), (PCHAR )mIotCoreRoleAlias.data(), (PCHAR )mThingName.data(), (PCHAR )mRegion.data(),
                                                              NULL, NULL, &pClientCallbacks);
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    MLogger::LOG(Level::DEBUG, "Init: createDefaultCallbacksProviderWithIotCertificate: %X, (duration=%d)", status, duration);

    // step 4: addFileLoggerPlatformCallbacksProvider();
    STATUS retStatus = STATUS_SUCCESS;
    if (NULL != getenv(ENABLE_FILE_LOGGING)) {
        if ((retStatus = addFileLoggerPlatformCallbacksProvider(pClientCallbacks, FILE_LOGGING_BUFFER_SIZE, MAX_NUMBER_OF_LOG_FILES,
                                                                (PCHAR) "/data/tmp/middleware/", TRUE) != STATUS_SUCCESS)) {
            printf("File logging enable option failed with 0x%08x error code\n", retStatus);
        }
    }

    // step 5: createStreamCallbacks();
    start = std::chrono::high_resolution_clock::now();
    status = createStreamCallbacks(&pStreamCallbacks);


    pStreamCallbacks->streamUnderflowReportFn = NULL;
    pStreamCallbacks->bufferDurationOverflowPressureFn = StreamCB_BufferDurationOverflowPressureFunc;
    pStreamCallbacks->streamLatencyPressureFn = StreamCB_StreamLatencyPressureFunc;
    pStreamCallbacks->streamConnectionStaleFn = StreamCB_StreamConnectionStaleFunc;
    pStreamCallbacks->droppedFrameReportFn = StreamCB_DroppedFrameReportFunc;
    pStreamCallbacks->droppedFragmentReportFn = StreamCB_DroppedFragmentReportFunc;
    pStreamCallbacks->streamErrorReportFn = StreamCB_StreamErrorReportFunc;
    pStreamCallbacks->fragmentAckReceivedFn = StreamCB_FragmentAckReceivedFunc;
    pStreamCallbacks->streamDataAvailableFn = StreamCB_StreamDataAvailableFunc;
    pStreamCallbacks->streamReadyFn = StreamCB_StreamReadyFunc;
    pStreamCallbacks->streamClosedFn = StreamCB_StreamClosedFunc;
    pStreamCallbacks->streamShutdownFn = StreamCB_StreamShutdownFunc;
    pStreamCallbacks->freeStreamCallbacksFn = StreamCB_FreeStreamCallbacksFunc;

    // Specialized cleanup callback
    FreeStreamCallbacksFunc freeStreamCallbacksFn;
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    MLogger::LOG(Level::DEBUG, "Init: Create Stream Callbacks: %x, (Duration=%d)", status, duration);

    // step 6: addStreamCallbacks();
    start = std::chrono::high_resolution_clock::now();
    status = addStreamCallbacks(pClientCallbacks, pStreamCallbacks);
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    MLogger::LOG(Level::DEBUG, "Init: Add Stream Callbacks: %x, (Duration=%d)", status, duration);

    // step 7: createKinesisVideoClient();
    start = std::chrono::high_resolution_clock::now();
    status = createKinesisVideoClient(sPDeviceInfo, pClientCallbacks, &clientHandle); // ToDo: Implement the callback functions
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    MLogger::LOG(Level::DEBUG, "Init: 7. Create Kinesis Video Client: status = %X, (Duration=%d)", status, duration);

    // step 8: createKinesisVideoStreamSync();
    start = std::chrono::high_resolution_clock::now();
    status = createKinesisVideoStreamSync(clientHandle, pStreamInfo, sPStreamHandle);
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    MLogger::LOG(Level::DEBUG, "Init: 8. createKinesisVideoStreamSync: status = %X, (Duration=%d)", status, duration);

    return 0;
}

int KvsProducer::Deinit() {
    //1
    auto start = std::chrono::high_resolution_clock::now();
    STATUS status = stopKinesisVideoStreamSync(*sPStreamHandle);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    MLogger::LOG(Level::DEBUG, "Deinit: stopKinesisVideoStreamSync: result = %X, (Duration=%d)", status, duration);
    //2
    start = std::chrono::high_resolution_clock::now();
    status = freeDeviceInfo(&mpDeviceInfo);
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    MLogger::LOG(Level::DEBUG, "Deinit: freeDeviceInfo: result = %X, (Duration=%d)", status, duration);
    // 3
    start = std::chrono::high_resolution_clock::now();
    status = freeStreamInfoProvider(&mpStreamInfo);
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    MLogger::LOG(Level::DEBUG, "Deinit: freeStreamInfoProvider: result = %X, (Duration=%d)", status, duration);

    // 4
    start = std::chrono::high_resolution_clock::now();
    status = freeKinesisVideoStream(sPStreamHandle);
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    MLogger::LOG(Level::DEBUG, "Deinit: freeKinesisVideoStream: result = %X, (Duration=%d)", status, duration);

    // 5
    start = std::chrono::high_resolution_clock::now();
    status = freeKinesisVideoClient(&mClientHandle);
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    MLogger::LOG(Level::DEBUG, "Deinit: freeKinesisVideoClient: result = %X, (Duration=%d)", status, duration);

    // 6
    start = std::chrono::high_resolution_clock::now();
    status = freeCallbacksProvider(&mpClientCallbacks); //ToDo: implement the callback functions
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    MLogger::LOG(Level::DEBUG, "Deinit: freeCallbacksProvider: result = %X, (Duration=%d)", status, duration);

    return 0;
}

int KvsProducer::SetStreamName(PCHAR name) {
    mStreamName = name;
    return 0;
}

STATUS KvsProducer::PutVideoFrame(PFrame pFrame) {
    static int i = 1;
    STATUS s = putKinesisVideoFrame(*sPStreamHandle, pFrame);
    if (i == 0) {
        StreamEventMetadata Meta{STREAM_EVENT_METADATA_CURRENT_VERSION, NULL, 1, {}, {}};
        CHAR tagName1[10] = {'\0'};
        CHAR tagValue1[10] = {'\0'};
        Meta.names[0] = tagName1;
        Meta.values[0] = tagValue1;
        MEMCPY(tagName1, (PCHAR) "TYPE", STRLEN("tagName"));
        MEMCPY(tagValue1, (PCHAR) "Value", STRLEN("tagValue"));
        STATUS s2 = putKinesisVideoEventMetadata(*sPStreamHandle,
                                                 STREAM_EVENT_TYPE_NOTIFICATION | STREAM_EVENT_TYPE_IMAGE_GENERATION,
                                                &Meta);
        MLogger::LOG(Level::DEBUG, "PutVideoFrame: result=0x%08x", s);
        MLogger::LOG(Level::DEBUG, "PutVideoFrame: putKinesisVideoEventMetadata: result=0x%08x", s2);
        i++;
    }
    return s;
}

STATUS KvsProducer::PutAudioFrame(PFrame pFrame) {
    STATUS s = putKinesisVideoFrame(*sPStreamHandle, pFrame);
    return s;
}

STATUS KvsProducer::PutAVFrame(PFrame pFrame) {
    STATUS s = putKinesisVideoFrame(*sPStreamHandle, pFrame);
    return s;
}
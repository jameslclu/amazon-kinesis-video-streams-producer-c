#include "KvsProducer.h"
#include "AllInterfaces.h"
#include "ComponentProvider.h"
#include "KvsServiceConfig.h"
#include "MLogger.h"

static PDeviceInfo mpDeviceInfo;
static PStreamInfo mpStreamInfo = NULL;
static PTrackInfo mpAudioTrack = NULL;
static PCHAR mStreamName;
static PClientCallbacks mpClientCallbacks = NULL;
static PStreamCallbacks mpStreamCallbacks = NULL;
static CLIENT_HANDLE mClientHandle = INVALID_CLIENT_HANDLE_VALUE;

static UINT64 gStartTime;
static UINT64 gStreamStartTime;
static UINT64 gFirstVideoPTS;
static UINT64 gFirstAudioPTS;
static UINT64 gStreamStopTime;
//static BOOL gIsFirstFrameSent;
static PSEMAPHORE_HANDLE mpStreamHandle;

static BYTE mAACAudioCpd[KVS_AAC_CPD_SIZE_BYTE];
static UINT8 gEventsEnabled = 0;
volatile ATOMIC_BOOL gIsFirstVideoFramePut = false;
static SampleStreamSource* psStreamSource;
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

static PDeviceInfo sPDeviceInfo;
#define DEFAULT_STORAGE_SIZE              20 * 1024 * 1024
#define RECORDED_FRAME_AVG_BITRATE_BIT_PS 3800000
static STREAM_HANDLE mStreamHandle = INVALID_STREAM_HANDLE_VALUE;
static PSTREAM_HANDLE sPStreamHandle = &mStreamHandle;
//static PCHAR sStreamName = (PCHAR) "SH20-eventStream-db-B813329BB08C";
//static PCHAR pIotCoreCredentialEndPoint = (PCHAR) "cne66nccv56pg.credentials.iot.ca-central-1.amazonaws.com";
//static PCHAR pIotCoreCert = (PCHAR) "/home/camera/kvs/cert";
//static PCHAR pIotCorePrivateKey = (PCHAR) "/home/camera/kvs/privkey";
//static PCHAR pCaCert = (PCHAR) "/home/camera/kvs/rootca.pem";
//static PCHAR pIotCoreRoleAlias = (PCHAR) "KvsCameraIoTRoleAlias";
//static PCHAR pThingName = (PCHAR) "db-B813329BB08C";
//static PCHAR pRegion = (PCHAR) "ca-central-1";
static PStreamInfo pStreamInfo = NULL;
static PTrackInfo spAudioTrack;
static BYTE sAACAudioCpd[KVS_AAC_CPD_SIZE_BYTE];
static PClientCallbacks pClientCallbacks = NULL;
static PStreamCallbacks pStreamCallbacks = NULL;
static CLIENT_HANDLE clientHandle = INVALID_CLIENT_HANDLE_VALUE;

int KvsProducer::Init() {
    mSettings.Init();
    // Step: 0
    mSettings.GetString(STREAM_NAME, mStreamName);
    mSettings.GetString(END_POINT, mIotCoreCredentialEndPoint);
    mSettings.GetString(CERT_LOCATION, mIotCoreCert);
    mSettings.GetString(KEY_LOCATION, mIotCorePrivateKey);
    mSettings.GetString(CA_LOCATION, mCaCert);
    mSettings.GetString(ROLE_ALIAS, mIotCoreRoleAlias);
    mSettings.GetString(THING_NAME, mThingName);
    mSettings.GetString(REGION, mRegion);

    STATUS status;
    createDefaultDeviceInfo(&sPDeviceInfo);
    sPDeviceInfo->clientInfo.loggerLogLevel = LOG_LEVEL_DEBUG;
    sPDeviceInfo->storageInfo.storageSize = DEFAULT_STORAGE_SIZE;

    // Step 1:
    status = createOfflineVideoStreamInfoProviderWithCodecs((PCHAR )mStreamName.data(), DEFAULT_RETENTION_PERIOD, DEFAULT_BUFFER_DURATION, VIDEO_CODEC_ID_H264,
                                                            &pStreamInfo);
    MLogger::LOG(Level::DEBUG, "createDefaultCallbacksProviderWithIotCertificate: %X", status);
    status = setStreamInfoBasedOnStorageSize(DEFAULT_STORAGE_SIZE, RECORDED_FRAME_AVG_BITRATE_BIT_PS, 1, pStreamInfo);
    MLogger::LOG(Level::DEBUG, "setStreamInfoBasedOnStorageSize: %X", status);

    pStreamInfo->streamCaps.absoluteFragmentTimes = FALSE;

    status = createDefaultCallbacksProviderWithIotCertificate((PCHAR )mIotCoreCredentialEndPoint.data(), (PCHAR )mIotCoreCert.data(), (PCHAR )mIotCorePrivateKey.data(),
                                                              (PCHAR )mCaCert.data(), (PCHAR )mIotCoreRoleAlias.data(), (PCHAR )mThingName.data(), (PCHAR )mRegion.data(),
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
    MLogger::LOG(Level::DEBUG, "8. createKinesisVideoStreamSync: result = %X", status);

    return 0;
}

//int KvsProducer::Init() {
//    createDefaultDeviceInfo(&mpDeviceInfo);
//    mpDeviceInfo->clientInfo.loggerLogLevel = LOG_LEVEL_DEBUG;
//    //Init-001
//    {
//        createRealtimeAudioVideoStreamInfoProviderWithCodecs(mStreamName, DEFAULT_RETENTION_PERIOD, DEFAULT_BUFFER_DURATION, VIDEO_CODEC_ID_H264,
//                                                             AUDIO_CODEC_ID_AAC, &mpStreamInfo);
//        // set up audio cpd.
//        mpAudioTrack = mpStreamInfo->streamCaps.trackInfoList[0].trackId == DEFAULT_AUDIO_TRACK_ID ? &mpStreamInfo->streamCaps.trackInfoList[0]
//                                                                                                   : &mpStreamInfo->streamCaps.trackInfoList[1];
//        mpAudioTrack->codecPrivateData = mAACAudioCpd;
//        mpAudioTrack->codecPrivateDataSize = KVS_AAC_CPD_SIZE_BYTE;
//        mkvgenGenerateAacCpd(AAC_LC, AAC_AUDIO_TRACK_SAMPLING_RATE, AAC_AUDIO_TRACK_CHANNEL_CONFIG, mpAudioTrack->codecPrivateData,
//                             mpAudioTrack->codecPrivateDataSize);
//    }
//    mpStreamInfo->streamCaps.absoluteFragmentTimes = FALSE;
//    // Init2
//    {
//        const PCHAR pIotCoreCredentialEndPoint = "cne66nccv56pg.credentials.iot.ca-central-1.amazonaws.com";
//        /*
//        PCHAR pIotCoreCert = "/media/sf_workspaces/kvs_files/cert";
//        PCHAR pIotCorePrivateKey = "/media/sf_workspaces/kvs_files/privkey";
//        PCHAR pCaCert = "/media/sf_workspaces/kvs_files/rootca.pem";
//        */
//        const PCHAR pIotCoreCert = "/tmp/kvs_files/cert";
//        const PCHAR pIotCorePrivateKey = "/tmp/kvs_files/privkey";
//        const PCHAR pCaCert = "/tmp/kvs_files/rootca.pem";
//        const PCHAR pIotCoreRoleAlias = "KvsCameraIoTRoleAlias";
//        const PCHAR pThingName = "db-B813329BB08C";
//        const PCHAR pRegion = "ca-central-1";
//        createDefaultCallbacksProviderWithIotCertificate(pIotCoreCredentialEndPoint, pIotCoreCert, pIotCorePrivateKey, pCaCert,
//                                                         pIotCoreRoleAlias, pThingName, pRegion, nullptr, nullptr, &mpClientCallbacks);
//    }
//    STATUS retStatus = STATUS_SUCCESS;
//    if (NULL != getenv(ENABLE_FILE_LOGGING)) {
//        if ((retStatus = addFileLoggerPlatformCallbacksProvider(mpClientCallbacks, FILE_LOGGING_BUFFER_SIZE, MAX_NUMBER_OF_LOG_FILES,
//                                                                (PCHAR) FILE_LOGGER_LOG_FILE_DIRECTORY_PATH, TRUE) != STATUS_SUCCESS)) {
//            printf("File logging enable option failed with 0x%08x error code\n", retStatus);
//        }
//    }
//    // Init3
//    {
//        createStreamCallbacks(&mpStreamCallbacks);
//        addStreamCallbacks(mpClientCallbacks, mpStreamCallbacks);
//    }
//    // Init4
//    createKinesisVideoClient(mpDeviceInfo, mpClientCallbacks, &mClientHandle);
//    createKinesisVideoStreamSync(mClientHandle, mpStreamInfo, mpStreamHandle);//&mStreamHandle);
//    return 0;
//}

int KvsProducer::Deinit() {
    stopKinesisVideoStreamSync(*mpStreamHandle);
    freeDeviceInfo(&mpDeviceInfo);
    freeStreamInfoProvider(&mpStreamInfo);
    freeKinesisVideoStream(mpStreamHandle);
    freeKinesisVideoClient(&mClientHandle);
    freeCallbacksProvider(&mpClientCallbacks);
    return 0;
}

int KvsProducer::SetStreamName(PCHAR name) {
    mStreamName = name;
    return 0;
}

STATUS KvsProducer::PutVideoFrame(PFrame pFrame) {
    return putKinesisVideoFrame(*sPStreamHandle, pFrame);
}

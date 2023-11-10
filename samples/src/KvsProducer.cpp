#include "KvsProducer.h"
#include "AllInterfaces.h"
#include "ComponentProvider.h"
#include "KvsServiceConfig.h"

static PDeviceInfo mpDeviceInfo;
static PStreamInfo mpStreamInfo = NULL;
static PTrackInfo mpAudioTrack = NULL;
static PCHAR mStreamName;
static PClientCallbacks mpClientCallbacks = NULL;
static PStreamCallbacks mpStreamCallbacks = NULL;
static CLIENT_HANDLE mClientHandle = INVALID_CLIENT_HANDLE_VALUE;

static UINT64 gStartTime;
static UINT64 gStreamStartTime;
static UINT64 gStreamStopTime;
//static BOOL gIsFirstFrameSent;
static PSEMAPHORE_HANDLE mpStreamHandle;

static BYTE mAACAudioCpd[KVS_AAC_CPD_SIZE_BYTE];
static UINT8 gEventsEnabled = 0;
volatile ATOMIC_BOOL gIsFirstVideoFramePut;
static StreamSource* psStreamSource;
PVOID putVideoFrameRoutine(PVOID args)
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
        status = putKinesisVideoFrame(psStreamSource->streamHandle, &frame);
        if (gIsFirstVideoFramePut) {
            startUpLatency = (DOUBLE) (GETTIME() - gStartTime) / (DOUBLE) HUNDREDS_OF_NANOS_IN_A_MILLISECOND;
            DLOGD("Start up latency: %lf ms", startUpLatency);
            gIsFirstVideoFramePut = FALSE;
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

PVOID putAudioFrameRoutine(PVOID args)
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
            status = putKinesisVideoFrame(psStreamSource->streamHandle, &frame);
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
TID audioSendTid, videoSendTid;

KvsProducer::KvsProducer() {}
KvsProducer::~KvsProducer() {}

int KvsProducer::SetDataSource(StreamSource* psource) {
    psStreamSource = psource;
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
}

int KvsProducer::SetHandler(STREAM_HANDLE* handler) {
    mpStreamHandle = handler;
    return 0;
}

int KvsProducer::Init() {
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
        PCHAR pIotCoreCert = "/media/sf_workspaces/kvs_files/cert";
        PCHAR pIotCorePrivateKey = "/media/sf_workspaces/kvs_files/privkey";
        PCHAR pCaCert = "/media/sf_workspaces/kvs_files/rootca.pem";
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

STATUS KvsProducer::PutVideoFrame(STREAM_HANDLE streamHandle, PFrame pFrame) {
    return putKinesisVideoFrame(*mpStreamHandle, pFrame);
    //return STATUS_SUCCESS;
}
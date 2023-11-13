#include <com/amazonaws/kinesis/video/cproducer/Include.h>
#include "src/StreamSource.h"
#include "src/KvsProducer.h"
#include "src/KvsServiceConfig.h"
#include "src/KvsService.h"
#include "src/KvsServiceStub.h"
#include "src/ComponentProvider.h"

STREAM_HANDLE mStreamHandle = INVALID_STREAM_HANDLE_VALUE;
int main() {
    StreamSource mStreamSource;
    KvsProducer mKvsProducer;

    // StreamSource Init
    // dbus-send --system --type=method_call --print-reply --dest=fxn.kvsservice /fxn/kvsservice fxn.kvsservice.cmd1 string:'{"key":"audio_speaker_volume"}'
    {
        static UINT64 streamingDuration = DEFAULT_STREAM_DURATION;
        mStreamSource.SetDataSource((PCHAR) "../samples");
        mStreamSource.Init();

        static PCHAR streamName = "SH20-eventStream-db-B813329BB08C";
        mKvsProducer.SetStreamName(streamName);

        mKvsProducer.SetHandler(&mStreamHandle);
        //mKvsProducer.Init();
        mStreamSource.SetHandler(mStreamHandle);

        ATOMIC_STORE_BOOL(&mStreamSource.firstVideoFramePut, FALSE);
    }

    mKvsProducer.SetDataSource(&mStreamSource);

    KvsService service(&mStreamSource, &mKvsProducer);
    service.Init();

    KvsServiceStub serviceStub(&service);

    ComponentProvider::GetInstance()->SetStreamSource(FAKE, &mStreamSource);
    ComponentProvider::GetInstance()->SetKvsRender(AWSPRODUCKER, &mKvsProducer);

//    ComponentProvider::GetInstance()->GetKvsPlayer(AWSPRODUCKER);
//    ComponentProvider::GetInstance()->GetStreamSource(FAKE);
    serviceStub.Init();
    (void)serviceStub.Deinit();
    (void)service.Deinit();
    //(void)MLogger::Instance().Deinit();
    return 0;

}
//INT32 main(INT32 argc, CHAR* argv[])
//{
//    STATUS retStatus = STATUS_SUCCESS;
//    TID audioSendTid, videoSendTid;
//    UINT64 streamingDuration = DEFAULT_STREAM_DURATION;
//    SampleCustomData data;
//    MEMSET(&data, 0x00, SIZEOF(SampleCustomData));
//    gStreamSource = &data;
//
//    mStreamSource.SetDataSource((PCHAR) "../samples");
//    mStreamSource.Init();
//
//    PCHAR streamName = "SH20-eventStream-db-B813329BB08C";
//    mKVSProducer.SetStreamName(streamName);
//
//    UINT64 streamStopTime = GETTIME() + streamingDuration;
//    mKVSProducer.Init();
//
//    mStreamSource.SetHandler(mStreamHandle);
//    mStreamSource.Init_Time(TRUE, GETTIME(), streamStopTime, GETTIME());
//    ATOMIC_STORE_BOOL(&gStreamSource->firstVideoFramePut, FALSE);
//
//    THREAD_CREATE(&videoSendTid, putVideoFrameRoutine, (PVOID) &gStreamSource);
//    THREAD_CREATE(&audioSendTid, putAudioFrameRoutine, (PVOID) &gStreamSource);
//
//    THREAD_JOIN(videoSendTid, NULL);
//    THREAD_JOIN(audioSendTid, NULL);
//
//CleanUp:
//    mStreamSource.Deinit();
//    mKVSProducer.Deinit();
//    mStreamSource.Deinit();
//    return (INT32) retStatus;
//}
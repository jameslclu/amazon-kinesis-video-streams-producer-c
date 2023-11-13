#include <com/amazonaws/kinesis/video/cproducer/Include.h>
#include "StreamSource.h"
#include "KvsProducer.h"
#include "KvsServiceConfig.h"
#include "KvsService.h"
#include "KvsServiceStub.h"
#include "ComponentProvider.h"

STREAM_HANDLE mStreamHandle = INVALID_STREAM_HANDLE_VALUE;
int main() {
    StreamSource mStreamSource;
    KvsProducer mKvsProducer;

    // StreamSource Init
    // dbus-send --system --type=method_call --print-reply --dest=fxn.kvsservice /fxn/kvsservice fxn.kvsservice.cmd1 string:'{"key":"audio_speaker_volume"}'
    {

        static UINT64 streamingDuration = DEFAULT_STREAM_DURATION;
        mStreamSource.SetDataSource((PCHAR) "../../samples");
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
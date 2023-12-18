#include <signal.h>
#include <json-c/json.h>

#ifdef CV28_BUILD
#include "FmspServer.h"
#include "OryxStreaming.h"
#include "player/KvsProducer.h"
#endif
#include "inc/FmspConfig.h"
#include "common/MLogger.h"
#include "james/JamesService.h"
#include "james/JamesServiceStub.h"
#include "player/ComponentProvider.h"
#include "player/SampleStreamSource.h"
#include "KvsProducer.h"
#include "oryx/OryxStreaming.h"
#include "SampleProducer.h"

JamesServiceStub* sp_ServiceStub;

static void signalHandler(int signal) {
    switch (signal) {
        case SIGINT:
        case SIGQUIT:
        case SIGKILL:
        case SIGTERM:
            if (sp_ServiceStub != nullptr) {
                (void) sp_ServiceStub->Interrupt(1);
            }
            break;
        default:
            break;
    }
}
#ifdef CV28_BUILD
int main2() {
    (void)MLogger::Instance().Init("MSProvider", "/data/tmp/middleware", "msprovider", 0, 10);
    MLogger::Instance().SetPrintLevel(Level::DEBUG);
    SampleProducer mSampleProducer;
    ComponentProvider::GetInstance()->SetKvsRender(EMPTY, &mSampleProducer);

    OryxStreaming oryxStreaming;
    oryxStreaming.Init();
    static Frame frame;
    for (int i=0; i<3; i++) {
        int status = oryxStreaming.GetVideoFrame(&frame.frameData, &frame.size, &frame.presentationTs);
        MLogger::LOG(Level::DEBUG, "GetVideoFrame: ret=%d", status);
    }
    (void)MLogger::Instance().Deinit();
    return 0;
}
#endif

int main() {
    (void)MLogger::Instance().Init("MSProvider", "/data/tmp/middleware", "msprovider", 0, 10);
    MLogger::Instance().SetPrintLevel(Level::DEBUG);

    (void)signal(SIGINT, signalHandler);
    (void)signal(SIGQUIT, signalHandler);
    (void)signal(SIGKILL, signalHandler);
    (void)signal(SIGTERM, signalHandler);

    int retStatus;

    KvsProducer mKvsProducer;
    //mKvsProducer.Init();
    ComponentProvider::GetInstance()->SetKvsRender(AWSPRODUCER, &mKvsProducer);

    SampleProducer mSampleProducer;
    ComponentProvider::GetInstance()->SetKvsRender(EMPTY, &mSampleProducer);

    SampleStreamSource mSampleStreamSource;
    mSampleStreamSource.SetDataSource("/home/camera/kvs/samples");
    mSampleStreamSource.Init();
    ComponentProvider::GetInstance()->SetStreamSource(FAKE, &mSampleStreamSource);

    retStatus = EXIT_FAILURE;
    JamesService service;
    JamesServiceStub serviceStub(&service);
    sp_ServiceStub = &serviceStub;

#ifdef CV28_BUILD
    OryxStreaming mOryxStreaming;
    mOryxStreaming.Init();
    ComponentProvider::GetInstance()->SetStreamSource(ORYX, &mOryxStreaming);
#endif

//    if( EXIT_SUCCESS == retStatus ) {
//        retStatus = FmspServerCreate();
//    }

    service.Init();
    MLogger::LOG(Level::INFO, "main: service stub init 2");
    serviceStub.Init();
    //MLogger::LOG(Level::INFO, "main: 5");
#ifdef CV28_BUILD
    //FmspServerDestroy();
#endif
    //OryxStreamingDestroy();

//    LibDBusClientDeInit();
//    LibUtilDeInit();

    (void)MLogger::Instance().Deinit();
    return EXIT_SUCCESS;
}
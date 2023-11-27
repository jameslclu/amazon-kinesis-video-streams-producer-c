#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>
#include <json-c/json.h>

//#include "LibFxnDBus.h"
//#include "LibFxnUtils.h"
//#include "FmspCommon.h"

#ifdef CV28_BUILD
#include "FmspServer.h"
#include "OryxStreaming.h"
#endif
#include "inc/FmspConfig.h"
#include "common/MLogger.h"
#include "james/JamesService.h"
#include "james/JamesServiceStub.h"
#include "player/ComponentProvider.h"
#include "player/SampleStreamSource.h"
#include "KvsProducer.h"

int main() {
    (void)MLogger::Instance().Init("MSProvider", "/data/tmp/middleware", "msprovider", 0, 10);
    MLogger::Instance().SetPrintLevel(Level::DEBUG);
    int retStatus;

    KvsProducer mKvsProducer;
    mKvsProducer.Init();
    ComponentProvider::GetInstance()->SetKvsRender(AWSPRODUCER, &mKvsProducer);

    SampleStreamSource mSampleStreamSource;
    mSampleStreamSource.SetDataSource("/home/camera/kvs/samples");
    mSampleStreamSource.Init();
    ComponentProvider::GetInstance()->SetStreamSource(FAKE, &mSampleStreamSource);

    retStatus = EXIT_FAILURE;
    JamesService service;
    JamesServiceStub serviceStub(&service);
    //if( LIB_UTILS_PROGRAM_ALREADY_EXECUTE ==
    //  LibUtilProgramIsExecute( FXN_PROGRAM_NAME ) )
    //  FXNLOGD( "%s Already Execute ..", FXN_PROGRAM_NAME );
    //else
    //{
        //LibUtilSystemSignalReg( &LibDBusClientLoopTerminate );

    //( void )LibUtilInit();
    //( void )LibUtilHWTimerOpen();

    //LibDBusClientInit();
#ifdef DOORBELL
    retStatus = OryxStreamingCreate();
#endif
    //}

//    if( EXIT_SUCCESS == retStatus ) {
//        retStatus = FmspServerCreate();
//    }

    //if( EXIT_SUCCESS == retStatus ) {
    //    LibDBusClientLoop();
    // }
    service.Init();
    MLogger::LOG(Level::INFO, "main: service stub init");
    serviceStub.Init();
    //MLogger::LOG(Level::INFO, "main: 5");
#ifdef CV28_BUILD
    FmspServerDestroy();
#endif
    //OryxStreamingDestroy();

//    LibDBusClientDeInit();
//    LibUtilDeInit();

    (void)MLogger::Instance().Deinit();
    return EXIT_SUCCESS;
}
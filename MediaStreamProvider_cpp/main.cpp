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
//#include "FmspServer.h"
//#include "OryxStreaming.h"
#endif
#include "inc/FmspConfig.h"
#include "common/MLogger.h"
#include "james/JamesService.h"
#include "james/JamesServiceStub.h"
#include "player/ComponentProvider.h"
#include "player/StreamSource.h"

int main() {
    (void)MLogger::Instance().Init("MSProvider", "/data/tmp/middleware", "msprovider", 0, 10);
    MLogger::Instance().SetPrintLevel(Level::DEBUG);
    int retStatus;
    StreamSource mStreamSource;
    mStreamSource.SetDataSource("/home/camera/kvs/samples");
    mStreamSource.Init();
    MLogger::LOG(Level::INFO, "main: 1");
    ComponentProvider::GetInstance()->SetStreamSource(FAKE, &mStreamSource);

    retStatus = EXIT_FAILURE;
    JamesService service;
    JamesServiceStub serviceStub(&service);
    MLogger::LOG(Level::INFO, "main: 2");
    //if( LIB_UTILS_PROGRAM_ALREADY_EXECUTE ==
    //  LibUtilProgramIsExecute( FXN_PROGRAM_NAME ) )
    //  FXNLOGD( "%s Already Execute ..", FXN_PROGRAM_NAME );
    //else
    //{
        //LibUtilSystemSignalReg( &LibDBusClientLoopTerminate );

    //( void )LibUtilInit();
    //( void )LibUtilHWTimerOpen();

    //LibDBusClientInit();
#ifdef CV28_BUILD
    retStatus = OryxStreamingCreate();
#endif
    MLogger::LOG(Level::INFO, "main: 3");
    //}

//    if( EXIT_SUCCESS == retStatus ) {
//        retStatus = FmspServerCreate();
//    }

    //if( EXIT_SUCCESS == retStatus ) {
    //    LibDBusClientLoop();
    // }
    service.Init();
    //MLogger::LOG(Level::INFO, "main: 4");
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
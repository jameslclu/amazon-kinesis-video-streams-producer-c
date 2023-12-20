#include "JamesService.h"
#include "MLogger.h"
#include "../player/KvsPlayer.h"

KvsPlayer sKvsPlayer;

JamesService::JamesService(): ServiceBase("JamesService") {
}

//JamesService::JamesService(StreamSource* pSource, KvsProducer* pProducer): ServiceBase("KvsService") {
//    //this->mpStreamSource = pSource;
//    //spStreamSource = pSource;
//    this->mpKvsProducer = pProducer;
//    spKvsProducer =  pProducer;
//}

JamesService::~JamesService() {}
static pthread_mutex_t m_mutex;

int JamesService::Init() {
    int result;
    // Create a thread and start it.
    (void) pthread_mutex_init(&m_mutex, nullptr);
    //(void) pthread_create(&m_thread_tid, nullptr, &ServiceA::innerThread, this);
    sKvsPlayer.Init();
    ServiceBaseInit();
    return result;
}

int JamesService::Deinit() {
    // joint the tread a thread and wait for stop
    int result;
    m_ThreadExit = true;
    (void) pthread_join(m_thread_tid, nullptr);
    ServiceBaseDeinit();
    sKvsPlayer.Deinit();
    return result;
}

bool JamesService::IsReady() {
    return 0;
}

int JamesService::cmd(std::string const &str, std::string &result) {
    return 0;
}

int JamesService::cmd1(std::string const &str, std::string &result) {
    MLogger::LOG(Level::DEBUG, "JamesService::cmd1");
    EnqueuenMethod(MethodItem("cmd1", "str"));
    return 0;
}

int JamesService::cmd2(std::string const &str, std::string &result) {
    EnqueuenMethod(MethodItem("cmd2", "str"));
    return 0;
}

int JamesService::HandleAsyncMethod(const MethodItem& method) {
    if ("cmd1" == method.m_method) {
        MLogger::LOG(Level::DEBUG, "JamesService::HandleAsyncMethod:cmd1: -> sKvsPlayer.Start");
        // sKvsPlayer.Start("device");
        sKvsPlayer.Start("pc-av");
        MLogger::LOG(Level::DEBUG, "JamesService::HandleAsyncMethod:cmd1: <- sKvsPlayer.Start");
        //sFakePlayer.Start();
    } else if ("cmd2" == method.m_method) {
        printf("cmd2");
        //sKvsPlayer.Stop();
        //sFakePlayer.Stop();
    }

    return 0;
}

int JamesService::GetAPIVersion(std::string &result) {
    return 0;
}

int JamesService::SetListener(IJamesServiceListener* plistener) {
    return 0;
}
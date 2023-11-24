#include "KvsService.h"
#include "KvsPlayer.h"
// StreamSource* spStreamSource;
KvsProducer* spKvsProducer;

KvsService::KvsService(): ServiceBase("KvsService") {
}

KvsService::KvsService(SampleStreamSource* pSource, KvsProducer* pProducer): ServiceBase("KvsService") {
    //this->mpStreamSource = pSource;
    //spStreamSource = pSource;
    this->mpKvsProducer = pProducer;
    spKvsProducer =  pProducer;
}

KvsService::~KvsService() {}
static pthread_mutex_t m_mutex;

int KvsService::Init() {
    int result;
    // Create a thread and start it.
    (void) pthread_mutex_init(&m_mutex, nullptr);
    //(void) pthread_create(&m_thread_tid, nullptr, &ServiceA::innerThread, this);
    ServiceBaseInit();
    return result;
}

int KvsService::Deinit() {
    // joint the tread a thread and wait for stop
    int result;
    m_ThreadExit = true;
    (void) pthread_join(m_thread_tid, nullptr);
    ServiceBaseDeinit();
    return result;
}

bool KvsService::IsReady() {
    return 0;
}

int KvsService::cmd(std::string const &str, std::string &result) {
    return 0;
}

int KvsService::cmd1(std::string const &str, std::string &result) {
    EnqueuenMethod(MethodItem("cmd1", "str"));
    return 0;
}

int KvsService::cmd2(std::string const &str, std::string &result) {
    EnqueuenMethod(MethodItem("cmd2", "str"));
    return 0;
}
#include "KvsPlayer.h"
static KvsPlayer sKvsPlayer;
int KvsService::HandleAsyncMethod(const MethodItem& method) {
    if ("cmd1" == method.m_method) {
        printf("cmd1");
        sKvsPlayer.Start();
        //mpKvsProducer->StartUpload();
        //mpStreamSource->Deinit();
        //mpKvsProducer->Deinit();

    } else if ("cmd2" == method.m_method) {
        printf("cmd2");
        sKvsPlayer.Stop();
        //mpKvsProducer->StartUpload();
        //mpStreamSource->Deinit();
        //mpKvsProducer->Deinit();

    }

    return 0;
}

int KvsService::GetAPIVersion(std::string &result) {
    return 0;
}

int KvsService::SetListener(IKvsServiceListener* plistener) {
    return 0;
}
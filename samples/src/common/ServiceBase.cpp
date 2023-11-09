#include "ServiceBase.h"
#include "MLogger.h"

void *ServiceBase::methodThread(void *arg) {
    auto *mt = reinterpret_cast<ServiceBase *>(arg);
    MethodItem job;
    pthread_detach(pthread_self());
    mt->mMethodTrhead_Running = true;

    while (!mt->mMethodTrhead_Stop) {
        // MLogger::LOG(Level::DEBUG,"methodThread(%s): -> Dequeue", mt->mName.c_str());
        if (mt->mMethodQueue.Dequeue(job)) {
            // MLogger::LOG(Level::DEBUG,"methodThread: -> HandleAsyncMethod");
            mt->HandleAsyncMethod(job);
        }
    }
    mt->mMethodTrhead_Running = false;
    pthread_exit(nullptr);
}

ServiceBase::ServiceBase(std::string name) {
    // MLogger::LOG(Level::DEBUG,"ServiceBase: %s", name.c_str());
    mName = std::move(name);
}

ServiceBase::~ServiceBase() = default;

int ServiceBase::ServiceBaseInit() {
    (void) pthread_create(&m_methodthread_tid, nullptr, &ServiceBase::methodThread, this);
    return 0;
}

int ServiceBase::ServiceBaseDeinit() {
    mMethodTrhead_Stop = true;
    for (int i=0;i < 10; i++) {
        if (!mMethodTrhead_Running) {
            break;
        }
    }
    return 0;
}

int ServiceBase::EnqueuenMethod(MethodItem methodItem) {
    mMethodQueue.Enqueue(methodItem);
    return 0;
}

/*int ServiceBase::EnqueuenResult(ResultItem resultItem) {
    mResultQueue.enqueue(resultItem);
    return 0;
}*/
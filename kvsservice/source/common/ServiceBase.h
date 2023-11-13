#ifndef FXN_MIDDLEWARE_SERVICEBASE_H
#define FXN_MIDDLEWARE_SERVICEBASE_H

#include "ResultQueue.h"
#include "MethodQueue.h"

using namespace std;

class ServiceBase {
public:
    MethodQueue mMethodQueue;
    ResultQueue mResultQueue;

    std::string mName;
    ServiceBase(std::string name);
    virtual ~ServiceBase();

    int ServiceBaseInit();
    int ServiceBaseDeinit();
    
    int EnqueuenMethod(MethodItem methodItem);

    pthread_t m_methodthread_tid;

    static void *methodThread(void *arg);

    virtual int HandleAsyncMethod(const MethodItem& method) = 0;
    
    // virtual int HandleResult(ResultItem result) { return 0;}

    bool mMethodTrhead_Stop = false;
    bool mMethodTrhead_Running = false;
};
#endif //FXN_MIDDLEWARE_SERVICEBASE_H
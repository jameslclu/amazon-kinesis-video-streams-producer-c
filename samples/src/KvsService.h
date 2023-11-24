#ifndef KVSPRODUCERSERVICE_KVSSERVICE_H
#define KVSPRODUCERSERVICE_KVSSERVICE_H

#include <string>
#include "IKvsService.h"
#include "common/ServiceBase.h"
#include "StreamSource.h"
#include "KvsProducer.h"

class KvsService : public IKvsService, public ServiceBase {
  public:
    SampleStreamSource* mpStreamSource;
    KvsProducer* mpKvsProducer;
    KvsService();
    KvsService(SampleStreamSource* pSource, KvsProducer* pProducer);

    virtual ~KvsService();

    int Init();

    int Deinit();

    bool IsReady();

    int cmd(std::string const &str, std::string &result);

    int cmd1(std::string const &str, std::string &result);

    int cmd2(std::string const &str, std::string &result);

    int GetAPIVersion(std::string &result);

    int SetListener(IKvsServiceListener* plistener);

    static void *innerThread(void *arg);

    static void *methodThread(void *arg);

    virtual int HandleAsyncMethod(const MethodItem& method);

    pthread_t m_thread_tid;
    pthread_t m_methodthread_tid;
    bool m_IsReady;
    bool m_IsDbusConnected = false;
    bool m_ThreadExit = false;
    bool m_ThreadRunning = false;

    IKvsServiceListener *mp_IServiceListener = NULL;
};

#endif // KVSPRODUCERSERVICE_KVSSERVICE_H

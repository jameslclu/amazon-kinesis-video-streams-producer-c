#ifndef JAMESSERVICE_H
#define JAMESSERVICE_H

#include <string>
#include "IJamesService.h"
#include "./../common/ServiceBase.h"

class JamesService : public IJamesService, public ServiceBase {
  public:
    JamesService();

    virtual ~JamesService();

    int Init();

    int Deinit();

    bool IsReady();

    int cmd(std::string const &str, std::string &result);

    int cmd1(std::string const &str, std::string &result);

    int cmd2(std::string const &str, std::string &result);

    int GetAPIVersion(std::string &result);

    int SetListener(IJamesServiceListener* plistener);

    static void *innerThread(void *arg);

    static void *methodThread(void *arg);

    virtual int HandleAsyncMethod(const MethodItem& method);

    pthread_t m_thread_tid;
    pthread_t m_methodthread_tid;
    bool m_IsReady;
    bool m_IsDbusConnected = false;
    bool m_ThreadExit = false;
    bool m_ThreadRunning = false;

    IJamesServiceListener *mp_IServiceListener = NULL;
};

#endif // JAMESSERVICE_H

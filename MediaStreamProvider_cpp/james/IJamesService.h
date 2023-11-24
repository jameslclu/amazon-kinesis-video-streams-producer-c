#ifndef IJAMESSERVICE_H
#define IJAMESSERVICE_H
#include <string>

class IJamesServiceListener {
  public:
    virtual int onJamesServiceCallback(std::string const &str) = 0;
};

class IJamesService {

  public:
    virtual int Init() = 0;

    virtual int Deinit() = 0;

    virtual bool IsReady() = 0;

    virtual int cmd1(std::string const &str, std::string &result) = 0;

    virtual int cmd2(std::string const &str, std::string &result) = 0;

    virtual int GetAPIVersion(std::string &result) = 0;

    virtual int SetListener(IJamesServiceListener* plistener)= 0;
};

#endif

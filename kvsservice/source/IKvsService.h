//
// Created by jh on 11/8/23.
//

#ifndef KVSPRODUCERSERVICE_IKVSSERVICE_H
#define KVSPRODUCERSERVICE_IKVSSERVICE_H
#include <string>

class IKvsServiceListener {
  public:
    virtual int onKvsServiceCallback(std::string const &str) = 0;
};

class IKvsService {

  public:
    virtual int Init() = 0;

    virtual int Deinit() = 0;

    virtual bool IsReady() = 0;

    virtual int cmd1(std::string const &str, std::string &result) = 0;

    virtual int cmd2(std::string const &str, std::string &result) = 0;

    virtual int GetAPIVersion(std::string &result) = 0;

    virtual int SetListener(IKvsServiceListener* plistener)= 0;
};

#endif // KVSPRODUCERSERVICE_IKVSSERVICE_H

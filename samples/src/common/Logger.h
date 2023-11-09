//
// Created by jh on 10/23/23.
//

#ifndef FXN_MIDDLEWARE_LOGGER_H
#define FXN_MIDDLEWARE_LOGGER_H

#include <string>

class Logger {
public:
    int mMaxSize;
    int mIndex;
    int mCheckSizeFrequencey;
    std::string mDir;
    std::string mName;
    std::string mFullPath;
    std::string mCurrentPath;
    std::string mIndexFile;
public:
    Logger(int sizelimit, std::string dir, std::string name, int freq): mMaxSize(sizelimit), mDir(dir), mName(name),
                                                                        mCheckSizeFrequencey(freq) {
        mFullPath = mDir + "/" + name;
        mIndexFile = mDir + "/index.txt";
    }
    virtual ~Logger() {}
    int Init();
    int Deinit();
    int Log(std::string log);
};


#endif //FXN_MIDDLEWARE_LOGGER_H

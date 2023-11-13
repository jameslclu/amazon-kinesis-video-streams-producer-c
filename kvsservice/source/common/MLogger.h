#ifndef FXN_MIDDLEWARE_MLOGGER_H
#define FXN_MIDDLEWARE_MLOGGER_H

#include <string>
#include <pthread.h>

enum class Level {
    EMERG   =0,
    ALERT	=1,
    CRIT	=2,
    ERROR	=3,
    WARN	=4,
    NOTICE	=5,
    INFO	=6,
    DEBUG	=7
};

class MLogger {
public:
    static std::string toLevelString(Level l);

public:

    MLogger() {}

    virtual ~MLogger() {}

    static MLogger &Instance();

    int SetPrintLevel(Level level);

    int Init(const char *psystemlogger, std::string const &dir, std::string const &fileName, int level, int maxFileSizes);

    int Deinit();

    void PrintLog(const std::string& msg);

    std::string GetTag();

    static void SYSLOG(Level level, const char *fmt, ...);

    static void LOG(Level level, const char *fmt, ...);

private:

    std::string m_dir;
    std::string m_filename;
    std::string Tag;
    int m_maxFileSize;
};

#endif //FXN_MIDDLEWARE_MLOGGER_H

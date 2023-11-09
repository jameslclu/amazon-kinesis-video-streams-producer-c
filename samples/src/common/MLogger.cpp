#include "MLogger.h"
#include <sys/syslog.h>
#include <fstream>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>
#include <mutex>

std::mutex myMutex;
static Level sPrintLevel = Level::INFO;

MLogger &MLogger::Instance() {
    static MLogger log;
    return log;
}

bool dirExists(const std::string &path) {
    bool v;
    struct stat info;
    bool exist = stat(path.c_str(), &info) == 1;
    if (!exist) {
        v = false;
    } else {
        v = info.st_mode & S_IFDIR;
    }
    return v;
}

long GetFileSize(const std::string path) {
    std::ifstream in_file(path, std::fstream::binary);
    (void) in_file.seekg(0, std::fstream::end);
    return in_file.tellg();
}

int MLogger::Init(const char *psystemlogger, std::string const &dir, std::string const &fileName, int level,
                  int maxFileSize) {
    openlog(psystemlogger, (LOG_CONS | LOG_PID | LOG_PERROR), LOG_USER);

    //mkdir_loop(std::string(dir).c_str(), 0755);

    m_dir = dir;
    m_filename = dir + "/" + fileName;
    m_maxFileSize = maxFileSize * 1024 * 1024;

    Tag = std::string(psystemlogger);
    return 0;
}

int MLogger::SetPrintLevel(Level level) {
    sPrintLevel = level;
    return 0;
}

int MLogger::Deinit() {
    closelog();
    return 0;
}

void MLogger::PrintLog(const std::string& msg) {

    std::fstream file;
    file.open(m_filename, std::fstream::out | std::fstream::app);
    (void) file.write(msg.c_str(), msg.size());
    if (file.fail()) {
        bool exist = dirExists(m_dir);
        syslog(LOG_DEBUG, "printLog: write: Folder(%s), exists = %d\n", m_dir.c_str(), exist);
        syslog(LOG_DEBUG, "printLog: write: Failed to open file\n");
    } else {
        file.close();
        if (GetFileSize(m_filename) > m_maxFileSize) {
            std::string newname = m_filename + "_backup";
            remove(newname.c_str());
            int result = rename(m_filename.c_str(), newname.c_str());
            if (result != 0) {
                syslog(LOG_DEBUG, "printLog: write: Failed to rename file\n");
            }
        }
    }
}
std::string MLogger::toLevelString(Level l) {
    std::string strLevel = "";
    switch(l) {
        case Level::EMERG:
            strLevel = "EMERG";
            break;
        case Level::ALERT:
            strLevel = "ALERT";
            break;
        case Level::CRIT:
            strLevel = "CRIT";
            break;
        case Level::ERROR:
            strLevel = "ERROR";
            break;
        case Level::WARN:
            strLevel = "WARNING";
            break;
        case Level::NOTICE:
            strLevel = "NOTICE";
            break;
        case Level::INFO:
            strLevel = "INFO";
            break;
        case Level::DEBUG:
            strLevel = "DEBUG";
            break;
    }
    return strLevel;
}


std::string MLogger::GetTag() {
    return Tag;
}

void MLogger::SYSLOG(Level level, const char *fmt, ...) {
    if (nullptr == fmt || level > sPrintLevel) {
        return;
    }
    std::lock_guard<std::mutex> guard(myMutex);

    std::string strlevel = toLevelString(level);
    static char printf_buf[1024] = {0};
    (void)memset(printf_buf, 0, 1024);
    va_list args;
    va_start(args, fmt);
    (void) vsnprintf(printf_buf, 1024, fmt, args);
    va_end(args);
    syslog(static_cast<int>(level), "%s", printf_buf);
}

void MLogger::LOG(Level level, const char *fmt, ...) {

    if (nullptr == fmt || level > sPrintLevel) {
        return;
    }
    std::lock_guard<std::mutex> guard(myMutex);


    static char printf_buf[1024] = {0};
    static char printf_whole[1088] = {0};

    {
        (void)memset(printf_buf, 0, 1024);
        va_list args;
        va_start(args, fmt);
        (void) vsnprintf(printf_buf, 1024, fmt, args);
        va_end(args);
        syslog(static_cast<int>(level), "%s", printf_buf);
    }

    {
        std::string strlevel = toLevelString(level);
        (void)memset(printf_whole, 0, 1088);
        time_t now = time(nullptr);
        struct tm *lt = localtime(&now);
        (void) snprintf(printf_whole, 1087, "%02d/%02d/%02d %02d:%02d:%02d [%s] %s: %s\n",
                        lt->tm_mon + 1, lt->tm_mday, lt->tm_year % 100, lt->tm_hour, lt->tm_min, lt->tm_sec, strlevel.c_str(),
                        MLogger::Instance().GetTag().c_str(), printf_buf);
        std::string msg(printf_whole);
        MLogger::Instance().PrintLog(msg);
    }
}
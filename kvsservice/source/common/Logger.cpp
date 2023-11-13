#include "../include/Logger.h"
#include <err.h>
#include <fstream>
#include "../include/popen_noshell.h"

int CyclingIncrease(int index) {
    index++;
    if (index >= 10) return 1;
    return index;
}

int CyclingDecrease(int index) {
    index--;
    if (index <= 0) return 9;
    return index;
}

int RemoveLogFile(std::string path, int index, bool onlyCurrent) {
    if (onlyCurrent) {
        std::string n = path + "/journal_" + std::to_string(index) + ".txt";
        std::remove(n.c_str());
        return 0;
    }

    int index1 = CyclingDecrease(index);
    int index2 = CyclingDecrease(index1);
    for (int i=1; i<10; i++) {
        if (i!=index && i!=index1 && i!=index2) {
            std::string n = path + "/journal_" + std::to_string(i) + ".txt";
            std::remove(n.c_str());
        }
    }
    return 0;
}

int fast_execute_cmd(const char* szCmd, char *szResult, u_int16_t size)
{
    int status;
    if (szCmd==NULL || szResult==NULL || size ==0U) {
        status = -1;
    } else {
        FILE *fp;
        struct popen_noshell_pass_to_pclose pclose_arg;

        // the command arguments used by popen_noshell()
        const char *exec_file = "/bin/bash";
        const char *arg1 = "-c";
        const char *endarg = NULL; // last element, must have
        const char *argv[] = {exec_file, arg1, szCmd, endarg}; // NOTE! The first argv[] must be the executed *exec_file itself

        fp = popen_noshell(exec_file, (const char * const *)argv, "r", &pclose_arg, 0);
        if (!fp) {
            err(EXIT_FAILURE, "popen_noshell_compat()");
        }

        size_t len = fread(szResult, sizeof(char), size-1U, fp);
        status = pclose_noshell(&pclose_arg);
        if (status == -1) {
            err(EXIT_FAILURE, "pclose_noshell()");
        }
    }
    return status;
}


int Logger::Init() {

    const unsigned short buf_size = 128U;
    char buf[buf_size] = {0};
    std::string cmd = "/bin/mkdir -p " + std::string(mDir);
    (void)fast_execute_cmd(cmd.c_str(), buf, buf_size*sizeof(char));

    // find biggest index fron index file
    // if index file doesn't existing. create one and put 1 and initial value
    std::fstream file;
    std::string indexstr;
    file.open(mIndexFile, std::fstream::in | std::fstream::app);
    std::getline(file, indexstr);
    file.close();

    if (indexstr.size() == 1 && indexstr.find_first_not_of("0123456789") == std::string::npos) {
        mIndex = stoi(indexstr);
    } else {
        file.open(mIndexFile, std::fstream::out | std::fstream::trunc);
        file<<"1\n";
        file.close();
        mIndex = 1;
    }

    return 0;
}

int Logger::Deinit()  {
    return 0;
}

static long GetFileSize(const std::string path) {
    std::ifstream in_file(path, std::fstream::binary);
    (void) in_file.seekg(0, std::fstream::end);
    return in_file.tellg();
}

int Logger::Log(std::string log) {
    //
    static int count = 0;

    std::fstream file;
    //std::string fname = mFullPath + std::to_string(mIndex) + ".txt";
    if (count > mCheckSizeFrequencey) {
        count = 0;

        long s = GetFileSize(mCurrentPath);
        if (s > mMaxSize) {
            mIndex = CyclingIncrease(mIndex);
            RemoveLogFile(mDir, mIndex, true);

            // write index back to the index file
            file.open(mIndexFile, std::fstream::out | std::fstream::trunc);
            file << mIndex << std::endl;
            file.close();

            // remove other files
            RemoveLogFile(mDir, mIndex, false);
        }
    } else {
        count++;
    }

    mCurrentPath  = mFullPath + std::to_string(mIndex) + ".txt";
    file.open(mCurrentPath, std::fstream::out | std::fstream::app);
    (void) file.write(log.c_str(), log.size());
    file.close();
}
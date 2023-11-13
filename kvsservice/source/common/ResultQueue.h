#ifndef FXN_MIDDLEWARE_RESULTQUEUE_H
#define FXN_MIDDLEWARE_RESULTQUEUE_H

#include <mutex>
#include <vector>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <thread>
#include <unordered_map>
#include <algorithm>

class ResultItem {
public:
    int m_seq;
    std::string m_method;
    std::string m_data;

    ResultItem() {}
    virtual ~ResultItem() {}

    ResultItem(std::string method, std::string data) {
        m_method = method;
        m_data = data;
    }

    ResultItem(ResultItem* pitem) {
        m_seq = pitem->m_seq;
        m_method = std::string (pitem->m_method);
        m_data = std::string (pitem->m_data);
    }

    void operator=(const ResultItem &actionItem) {
        this->m_seq = actionItem.m_seq;
        this->m_method = std::string(actionItem.m_method);
        this->m_data = std::string(actionItem.m_data);
    }

    bool operator==(const ResultItem &actionItem) const {
        return (this->m_method == actionItem.m_method);
    }

    bool operator==(const std::string method) const {
        return (this->m_method == method);
    }
};

class ResultQueue {
public:
    std::vector<ResultItem> job_Q;

    ResultQueue() {}
    virtual ~ResultQueue() {}

    bool enqueue(const ResultItem &job) {
        //remove_lookup(job.sn_);
        std::unique_lock<std::mutex> lock(queue_mutex);
        job_Q.push_back(job);
        condition.notify_one();
        return true;
    }

    bool dequeue(ResultItem &job) {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (job_Q.empty()) return false;

        job = std::move(job_Q.front());
        job_Q.erase(job_Q.begin());
        condition.notify_one();
        return true;
    }

    bool remove_lookup(std::string sn) {
        std::unique_lock<std::mutex> lock(queue_mutex);
        auto i = find(job_Q.begin(), job_Q.end(), sn);
        if (i != job_Q.end()) {
            job_Q.erase(i);
        }
        condition.notify_one();
        return true;
    }

public:
    std::mutex queue_mutex;
    std::condition_variable condition;
};

#endif
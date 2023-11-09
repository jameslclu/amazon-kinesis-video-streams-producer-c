#ifndef FXN_MIDDLEWARE_METHODQUEUE_H
#define FXN_MIDDLEWARE_METHODQUEUE_H

#include <mutex>
#include <vector>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <thread>
#include <unordered_map>
#include <algorithm>

class MethodItem {
public:
    std::string m_method;
    std::string m_data;

public:
    MethodItem();
    virtual ~MethodItem();

    MethodItem(std::string method, std::string data);

    explicit MethodItem(MethodItem* pitem);

    MethodItem& operator=(const MethodItem &methodItem) {
        this->m_method = std::string(methodItem.m_method);
        this->m_data = std::string(methodItem.m_data);
        return *this;
    }

    bool operator==(const MethodItem &methodItem) const {
        return (this->m_method == methodItem.m_method)
               && (this->m_data == methodItem.m_data);
    }

    bool operator==(const std::string& method) const {
        return (this->m_method == method);
    }
};

class MethodQueue {
public:
    MethodQueue();

    virtual ~MethodQueue();

    bool Enqueue(const MethodItem &item);

    bool Dequeue(MethodItem &item);

    bool Finds(const MethodItem &item);

public:
    std::vector<MethodItem> mQueue;
    std::mutex mQueueMutex;
    std::condition_variable mConditionVar;
    bool mStop = false;
};
#endif //FXN_MIDDLEWARE_METHODQUEUE_H

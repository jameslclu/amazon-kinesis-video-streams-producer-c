#include "MethodQueue.h"

#include <mutex>
#include <atomic>
#include <unordered_map>
#include <utility>

MethodItem::MethodItem() = default;

MethodItem::~MethodItem() = default;

MethodItem::MethodItem(std::string method, std::string data) {
    m_method = std::move(method);
    m_data = std::move(data);
}

MethodItem::MethodItem(MethodItem* pitem) {
    m_method = std::string (pitem->m_method);
    m_data = std::string (pitem->m_data);
}

MethodQueue::MethodQueue() = default;

MethodQueue::~MethodQueue() {
    {
        std::unique_lock<std::mutex> lock(mQueueMutex);
        mStop = true;
    }
    mConditionVar.notify_all();
    mQueue.clear();
}

bool MethodQueue::Finds(const MethodItem &item) {
//    bool found = false;
//    if (queue_mutex.try_lock_for(std::chrono::seconds(3))) {
//        for (const MethodItem& im: job_Q) {
//            if (im == item) {
//                break;
//            }
//        }
//        queue_mutex.unlock();
//        found = true;
//    }
//    return found;
}

bool MethodQueue::Enqueue(const MethodItem &item) {
    std::unique_lock<std::mutex> lock(mQueueMutex);
    mQueue.push_back(item);
    mConditionVar.notify_one();
    return true;
}

bool MethodQueue::Dequeue(MethodItem &item) {
    std::unique_lock<std::mutex> lock(mQueueMutex);
    if (mQueue.empty()) {
        mConditionVar.wait(lock, [this] {
            return this->mStop || !this->mQueue.empty();});
    }

    if (this->mQueue.empty()) {
        return false;
    }

    if (mStop) {
        mQueue.clear();
        return false;
    }
    item = mQueue.front();
    mQueue.erase(mQueue.begin());
    return true;
}
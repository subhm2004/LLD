#ifndef LRU_CACHE_LLD_UTILS_CONCURRENCYHELPERS_H
#define LRU_CACHE_LLD_UTILS_CONCURRENCYHELPERS_H

#include <condition_variable>
#include <mutex>

namespace lru_cache_lld {

class CyclicBarrier {
public:
    explicit CyclicBarrier(int participantCount)
        : total_(participantCount), waiting_(participantCount) {}

    void arriveAndWait() {
        std::unique_lock<std::mutex> lock(mutex_);
        if (--waiting_ == 0) {
            released_ = true;
            condition_.notify_all();
            return;
        }

        condition_.wait(lock, [this] { return released_; });
    }

private:
    int total_;
    int waiting_;
    bool released_ = false;
    std::mutex mutex_;
    std::condition_variable condition_;
};

class CountDownLatch {
public:
    explicit CountDownLatch(int count) : count_(count) {}

    void countDown() {
        std::unique_lock<std::mutex> lock(mutex_);
        if (count_ > 0) {
            --count_;
        }
        if (count_ == 0) {
            condition_.notify_all();
        }
    }

    void await() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this] { return count_ == 0; });
    }

private:
    int count_;
    std::mutex mutex_;
    std::condition_variable condition_;
};

} // namespace lru_cache_lld

#endif // LRU_CACHE_LLD_UTILS_CONCURRENCYHELPERS_H

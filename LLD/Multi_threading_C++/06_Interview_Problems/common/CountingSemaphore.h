#ifndef MULTI_THREADING_INTERVIEW_COMMON_COUNTINGSEMAPHORE_H
#define MULTI_THREADING_INTERVIEW_COMMON_COUNTINGSEMAPHORE_H

#include <condition_variable>
#include <mutex>

namespace interview_common {

/** C++17 counting semaphore (C++20 has std::counting_semaphore). */
class CountingSemaphore {
public:
    explicit CountingSemaphore(int permits) : permits_(permits) {}

    void acquire() {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] { return permits_ > 0; });
        --permits_;
    }

    void release() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            ++permits_;
        }
        cv_.notify_one();
    }

private:
    std::mutex mtx_;
    std::condition_variable cv_;
    int permits_;
};

}  // namespace interview_common

#endif  // MULTI_THREADING_INTERVIEW_COMMON_COUNTINGSEMAPHORE_H

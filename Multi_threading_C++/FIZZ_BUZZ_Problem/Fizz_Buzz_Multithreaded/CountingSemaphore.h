#ifndef MULTI_THREADING_FIZZ_BUZZ_COUNTINGSEMAPHORE_H
#define MULTI_THREADING_FIZZ_BUZZ_COUNTINGSEMAPHORE_H

#include <condition_variable>
#include <mutex>

namespace fizz_buzz_demo {

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

}  // namespace fizz_buzz_demo

#endif  // MULTI_THREADING_FIZZ_BUZZ_COUNTINGSEMAPHORE_H

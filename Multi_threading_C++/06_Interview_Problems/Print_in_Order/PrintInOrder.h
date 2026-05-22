#ifndef MULTI_THREADING_PRINT_IN_ORDER_H
#define MULTI_THREADING_PRINT_IN_ORDER_H

#include <condition_variable>
#include <functional>
#include <mutex>

namespace print_in_order {

/** LeetCode 1114 — print first, second, third in order. */
class PrintInOrder {
public:
    void first(std::function<void()> printFirst) {
        std::lock_guard<std::mutex> lock(mtx_);
        printFirst();
        first_done_ = true;
        cv_.notify_all();
    }

    void second(std::function<void()> printSecond) {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] { return first_done_; });
        printSecond();
        second_done_ = true;
        cv_.notify_all();
    }

    void third(std::function<void()> printThird) {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] { return second_done_; });
        printThird();
    }

private:
    std::mutex mtx_;
    std::condition_variable cv_;
    bool first_done_{false};
    bool second_done_{false};
};

}  // namespace print_in_order

#endif  // MULTI_THREADING_PRINT_IN_ORDER_H

#ifndef MULTI_THREADING_PRINT_ZERO_EVEN_ODD_H
#define MULTI_THREADING_PRINT_ZERO_EVEN_ODD_H

#include <condition_variable>
#include <functional>
#include <mutex>

namespace print_zero_even_odd {

/**
 * LeetCode 1116 — print series: 0, even, 0, odd, 0, even, ...
 * For n=2 → 0102
 */
class ZeroEvenOdd {
public:
    explicit ZeroEvenOdd(int n) : n_(n), next_(1) {}

    void zero(std::function<void(int)> printNumber) {
        for (int i = 0; i < n_; ++i) {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this] { return turn_ == Turn::ZERO; });
            printNumber(0);
            turn_ = (next_ % 2 == 0) ? Turn::EVEN : Turn::ODD;
            cv_.notify_all();
        }
    }

    void even(std::function<void(int)> printNumber) {
        for (int val = 2; val <= n_; val += 2) {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this] { return turn_ == Turn::EVEN; });
            printNumber(val);
            ++next_;
            turn_ = Turn::ZERO;
            cv_.notify_all();
        }
    }

    void odd(std::function<void(int)> printNumber) {
        for (int val = 1; val <= n_; val += 2) {
            std::unique_lock<std::mutex> lock(mtx_);
            cv_.wait(lock, [this] { return turn_ == Turn::ODD; });
            printNumber(val);
            ++next_;
            turn_ = Turn::ZERO;
            cv_.notify_all();
        }
    }

private:
    enum class Turn { ZERO, EVEN, ODD };

    int n_;
    int next_;
    Turn turn_{Turn::ZERO};
    std::mutex mtx_;
    std::condition_variable cv_;
};

}  // namespace print_zero_even_odd

#endif  // MULTI_THREADING_PRINT_ZERO_EVEN_ODD_H

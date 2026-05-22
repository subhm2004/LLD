#ifndef MULTI_THREADING_FIZZ_BUZZ_MULTITHREADED_FIZZBUZZ_H
#define MULTI_THREADING_FIZZ_BUZZ_MULTITHREADED_FIZZBUZZ_H

#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>

namespace fizz_buzz_demo {

enum class TurnKind { NUMBER, FIZZ, BUZZ, FIZZBUZZ };

inline TurnKind turnFor(int value) {
    if (value % 15 == 0) {
        return TurnKind::FIZZBUZZ;
    }
    if (value % 3 == 0) {
        return TurnKind::FIZZ;
    }
    if (value % 5 == 0) {
        return TurnKind::BUZZ;
    }
    return TurnKind::NUMBER;
}

/**
 * LeetCode 411 style API — four threads call fizz/buzz/fizzbuzz/number.
 * Uses one mutex + condition_variable (Solution 3).
 */
class FizzBuzz {
public:
    explicit FizzBuzz(int n) : n_(n), current_(1) {}

    void fizz(std::function<void()> printFizz) {
        while (runTurn(TurnKind::FIZZ, [&]() {
                   printFizz();
                   std::cout << "fizz,";
               })) {
        }
    }

    void buzz(std::function<void()> printBuzz) {
        while (runTurn(TurnKind::BUZZ, [&]() {
                   printBuzz();
                   std::cout << "buzz,";
               })) {
        }
    }

    void fizzbuzz(std::function<void()> printFizzBuzz) {
        while (runTurn(TurnKind::FIZZBUZZ, [&]() {
                   printFizzBuzz();
                   std::cout << "fizzbuzz,";
               })) {
        }
    }

    void number(std::function<void(int)> printNumber) {
        while (runTurn(TurnKind::NUMBER, [&]() {
                   printNumber(current_);
                   std::cout << current_ << ",";
               })) {
        }
    }

private:
    /** @return false when all values 1..n are printed */
    bool runTurn(TurnKind expected, const std::function<void()>& action) {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [&]() { return current_ > n_ || turnFor(current_) == expected; });
        if (current_ > n_) {
            cv_.notify_all();
            return false;
        }
        action();
        ++current_;
        cv_.notify_all();
        return current_ <= n_;
    }

    int n_;
    int current_;
    std::mutex mutex_;
    std::condition_variable cv_;
};

}  // namespace fizz_buzz_demo

#endif  // MULTI_THREADING_FIZZ_BUZZ_MULTITHREADED_FIZZBUZZ_H

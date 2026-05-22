/**
 * 04 — Spin lock built with CAS (compare_exchange on atomic<bool>).
 */
#include <atomic>
#include <iostream>
#include <thread>
#include <vector>

#include "SpinLock.h"

using namespace std;

int main() {
    cout << "====== 04 CAS Spin Lock ======\n\n";

    SpinLock lock;
    int shared = 0;
    constexpr int kThreads = 8;
    constexpr int kOps = 50'000;

    vector<thread> workers;
    workers.reserve(kThreads);
    for (int t = 0; t < kThreads; ++t) {
        workers.emplace_back([&]() {
            for (int i = 0; i < kOps; ++i) {
                lock.lock();
                ++shared;
                lock.unlock();
            }
        });
    }
    for (auto& th : workers) {
        th.join();
    }

    cout << "Expected shared=" << (kThreads * kOps) << ", actual=" << shared << "\n";
    cout << (shared == kThreads * kOps ? "PASS: no lost updates\n" : "FAIL: race detected\n");

    cout << "\nTrade-off: spin lock busy-waits — good for very short critical sections;\n";
    cout << "mutex usually better when hold time is long or thread count is high.\n";
    return 0;
}

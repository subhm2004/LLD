/**
 * 02 — Mutex vs atomic (lock-free counter) under contention.
 */
#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

using namespace std;
using Clock = chrono::steady_clock;

static constexpr int kThreads = 4;
static constexpr int kIncrementsPerThread = 250'000;

long long runMutexCounter() {
    long long counter = 0;
    mutex m;
    vector<thread> workers;
    workers.reserve(kThreads);

    auto start = Clock::now();
    for (int t = 0; t < kThreads; ++t) {
        workers.emplace_back([&]() {
            for (int i = 0; i < kIncrementsPerThread; ++i) {
                lock_guard<mutex> lock(m);
                ++counter;
            }
        });
    }
    for (auto& th : workers) {
        th.join();
    }
    auto ms = chrono::duration_cast<chrono::milliseconds>(Clock::now() - start).count();
    cout << "  mutex counter=" << counter << " time_ms=" << ms << "\n";
    return ms;
}

long long runAtomicCounter() {
    atomic<long long> counter{0};
    vector<thread> workers;
    workers.reserve(kThreads);

    auto start = Clock::now();
    for (int t = 0; t < kThreads; ++t) {
        workers.emplace_back([&]() {
            for (int i = 0; i < kIncrementsPerThread; ++i) {
                counter.fetch_add(1, memory_order_relaxed);
            }
        });
    }
    for (auto& th : workers) {
        th.join();
    }
    auto ms = chrono::duration_cast<chrono::milliseconds>(Clock::now() - start).count();
    cout << "  atomic counter=" << counter.load() << " time_ms=" << ms << "\n";
    return ms;
}

int main() {
    cout << "====== 02 Mutex vs Atomic Counter ======\n\n";
    cout << "Threads=" << kThreads << ", increments/thread=" << kIncrementsPerThread << "\n\n";

    cout << "[mutex]\n";
    runMutexCounter();

    cout << "\n[atomic fetch_add]\n";
    runAtomicCounter();

    cout << "\nNote: timings vary by CPU/OS. Atomic avoids mutex blocking; "
            "still has cache-line contention.\n";
    return 0;
}

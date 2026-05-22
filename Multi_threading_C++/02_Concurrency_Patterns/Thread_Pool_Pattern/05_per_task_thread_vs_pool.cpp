/**
 * 05 — Per-task thread creation vs thread pool (timing demo).
 */
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "ThreadPool.h"

using namespace std;

constexpr int kJobs = 40;

void small_job(int id) {
    (void)id;
    volatile int x = 0;
    for (int i = 0; i < 5000; ++i) {
        x += i;
    }
}

long long demo_per_task_threads() {
    auto start = chrono::steady_clock::now();
    vector<thread> threads;
    threads.reserve(kJobs);
    for (int i = 0; i < kJobs; ++i) {
        threads.emplace_back(small_job, i);
    }
    for (auto& t : threads) {
        t.join();
    }
    return chrono::duration_cast<chrono::milliseconds>(
               chrono::steady_clock::now() - start)
        .count();
}

long long demo_thread_pool() {
    auto start = chrono::steady_clock::now();
    ThreadPool pool(4);
    for (int i = 0; i < kJobs; ++i) {
        pool.enqueue([i]() { small_job(i); });
    }
    pool.shutdown();
    return chrono::duration_cast<chrono::milliseconds>(
               chrono::steady_clock::now() - start)
        .count();
}

int main() {
    cout << "====== 05 Per-Task Thread vs Pool ======\n";
    cout << "Jobs=" << kJobs << " (light CPU work each)\n\n";

    long long ms_threads = demo_per_task_threads();
    cout << "[per-task threads] " << ms_threads << " ms\n";

    long long ms_pool = demo_thread_pool();
    cout << "[thread pool x4]  " << ms_pool << " ms\n\n";

    cout << "Pool reuses workers — avoids " << kJobs
         << " create/join cycles.\n";
    cout << "(Exact numbers vary by machine; pool usually wins for many small tasks.)\n";
    return 0;
}

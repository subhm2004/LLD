/**
 * 04 — Graceful shutdown: stop flag + notify_all, workers drain queue then exit.
 */
#include <chrono>
#include <iostream>
#include <thread>

#include "ThreadPool.h"

using namespace std;

int main() {
    cout << "====== 04 Graceful Shutdown ======\n\n";

    ThreadPool pool(3);

    for (int i = 1; i <= 5; ++i) {
        pool.enqueue([i]() {
            cout << "[task " << i << "] processing...\n";
            this_thread::sleep_for(chrono::milliseconds(300));
            cout << "[task " << i << "] complete\n";
        });
    }

    this_thread::sleep_for(chrono::milliseconds(400));
    cout << "\n[main] calling pool.shutdown() — workers finish pending tasks\n";
    pool.shutdown();

    cout << "[main] pool stopped=" << (pool.is_stopped() ? "true" : "false")
         << ", workers=" << pool.worker_count() << "\n";
    cout << "No dangling threads — safe to exit.\n";
    return 0;
}

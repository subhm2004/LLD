/**
 * 01 — Basic thread pool: fixed workers, enqueue lambdas, graceful destructor shutdown.
 */
#include <chrono>
#include <iostream>
#include <thread>

#include "ThreadPool.h"

using namespace std;

int main() {
    cout << "====== 01 Basic Thread Pool ======\n\n";

    ThreadPool pool(3);
    cout << "[main] pool started with " << pool.worker_count() << " workers\n\n";

    for (int i = 1; i <= 6; ++i) {
        pool.enqueue([i]() {
            cout << "[task " << i << "] running on thread "
                 << this_thread::get_id() << "\n";
            this_thread::sleep_for(chrono::milliseconds(150));
            cout << "[task " << i << "] done\n";
        });
    }

    this_thread::sleep_for(chrono::seconds(2));
    cout << "\n[main] pool destructor will shutdown workers...\n";
    return 0;
}

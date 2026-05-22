/**
 * 03 — Thread reuse: same worker thread IDs appear across multiple tasks.
 */
#include <chrono>
#include <iostream>
#include <mutex>
#include <sstream>
#include <thread>
#include <unordered_map>

#include "ThreadPool.h"

using namespace std;

mutex print_mtx;
unordered_map<thread::id, int> id_to_worker;

int main() {
    cout << "====== 03 Thread Reuse ======\n\n";

    ThreadPool pool(3);

    for (int i = 1; i <= 12; ++i) {
        pool.enqueue([i]() {
            thread::id tid = this_thread::get_id();
            int label = -1;
            {
                lock_guard<mutex> lock(print_mtx);
                if (!id_to_worker.count(tid)) {
                    label = static_cast<int>(id_to_worker.size()) + 1;
                    id_to_worker[tid] = label;
                } else {
                    label = id_to_worker[tid];
                }
                cout << "[task " << i << "] worker#" << label
                     << " thread_id=" << tid << "\n";
            }
            this_thread::sleep_for(chrono::milliseconds(50));
        });
    }

    this_thread::sleep_for(chrono::seconds(2));
    cout << "\nUnique worker threads used: " << id_to_worker.size()
         << " (should be <= 3)\n";
    return 0;
}

/**
 * 02 — Many tasks, few workers: extra tasks wait in queue (FIFO).
 */
#include <chrono>
#include <iostream>
#include <thread>

#include "ThreadPool.h"

using namespace std;

int main() {
    cout << "====== 02 Many Tasks Queued ======\n\n";

    const int kWorkers = 2;
    const int kTasks = 10;

    ThreadPool pool(kWorkers);
    cout << "Workers=" << kWorkers << ", tasks=" << kTasks << "\n\n";

    for (int i = 1; i <= kTasks; ++i) {
        pool.enqueue([i]() {
            cout << "[task " << i << "] start (thread "
                 << this_thread::get_id() << ")\n";
            this_thread::sleep_for(chrono::milliseconds(200));
            cout << "[task " << i << "] finish\n";
        });
        cout << "[main] enqueued task " << i
             << ", pending in queue ~" << pool.pending_tasks() << "\n";
    }

    this_thread::sleep_for(chrono::seconds(3));
    cout << "\nAll tasks should complete before pool exits.\n";
    return 0;
}

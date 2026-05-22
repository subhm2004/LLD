/**
 * 03 — Semaphore as signaling: N permits = N slots available.
 * C++17 custom counting semaphore (C++20 has std::counting_semaphore).
 */
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

namespace signaling_demo {

class CountingSemaphore {
public:
    explicit CountingSemaphore(int permits) : permits_(permits) {}

    void acquire() {
        unique_lock<mutex> lock(mtx_);
        cv_.wait(lock, [this] { return permits_ > 0; });
        --permits_;
    }

    void release() {
        {
            lock_guard<mutex> lock(mtx_);
            ++permits_;
        }
        cv_.notify_one();
    }

private:
    mutex mtx_;
    condition_variable cv_;
    int permits_;
};

constexpr int kPoolSize = 3;

CountingSemaphore pool(kPoolSize);

void use_connection(int worker_id, int task_id) {
    cout << "[W" << worker_id << "] task " << task_id
         << " waiting for connection...\n";
    pool.acquire();
    cout << "[W" << worker_id << "] task " << task_id
         << " got connection (using 500ms)\n";
    this_thread::sleep_for(chrono::milliseconds(500));
    pool.release();
    cout << "[W" << worker_id << "] task " << task_id
         << " released connection\n";
}

}  // namespace signaling_demo

int main() {
    using namespace signaling_demo;

    cout << "Connection pool size = " << kPoolSize
         << " (semaphore signals slot free)\n\n";

    thread workers[5];
    for (int i = 0; i < 5; ++i) {
        workers[i] = thread(use_connection, i, i);
    }
    for (auto& w : workers) {
        w.join();
    }

    cout << "\nSemaphore signaling complete.\n";
    return 0;
}

/**
 * 05 — Graceful shutdown: set done flag + notify_all so all worker threads exit.
 */
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

using namespace std;

namespace signaling_demo {

struct TaskQueue {
    mutex mtx;
    condition_variable cv;
    queue<int> tasks;
    bool shutdown = false;

    void push(int t) {
        {
            lock_guard<mutex> lock(mtx);
            tasks.push(t);
        }
        cv.notify_one();
    }

    bool pop(int& out) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [this] { return !tasks.empty() || shutdown; });
        if (tasks.empty() && shutdown) {
            return false;
        }
        out = tasks.front();
        tasks.pop();
        return true;
    }

    void request_shutdown() {
        {
            lock_guard<mutex> lock(mtx);
            shutdown = true;
        }
        cv.notify_all();
    }
};

void worker(int id, TaskQueue& q) {
    int task = 0;
    while (q.pop(task)) {
        cout << "[worker " << id << "] processing task " << task << "\n";
        this_thread::sleep_for(chrono::milliseconds(100));
    }
    cout << "[worker " << id << "] exiting (shutdown signaled)\n";
}

}  // namespace signaling_demo

int main() {
    using namespace signaling_demo;

    TaskQueue q;
    const int kWorkers = 3;

    vector<thread> pool;
    for (int i = 0; i < kWorkers; ++i) {
        pool.emplace_back(worker, i, ref(q));
    }

    for (int t = 1; t <= 6; ++t) {
        q.push(t);
    }

    this_thread::sleep_for(chrono::milliseconds(400));
    cout << "[main] signaling shutdown to all workers...\n";
    q.request_shutdown();

    for (auto& t : pool) {
        t.join();
    }
    cout << "Shutdown signaling complete.\n";
    return 0;
}

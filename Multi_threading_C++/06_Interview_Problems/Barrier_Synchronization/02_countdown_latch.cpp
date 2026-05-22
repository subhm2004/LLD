/**
 * 02 — CountDownLatch: one thread waits until N workers signal done.
 */
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

using namespace std;

class CountDownLatch {
public:
    explicit CountDownLatch(int count) : count_(count) {}

    void countDown() {
        lock_guard<mutex> lock(mtx_);
        if (count_ > 0 && --count_ == 0) {
            cv_.notify_all();
        }
    }

    void await() {
        unique_lock<mutex> lock(mtx_);
        cv_.wait(lock, [this] { return count_ == 0; });
    }

private:
    mutex mtx_;
    condition_variable cv_;
    int count_;
};

int main() {
    cout << "====== CountDownLatch (main waits for 3 workers) ======\n";

    CountDownLatch latch(3);

    vector<thread> workers;
    for (int i = 0; i < 3; ++i) {
        workers.emplace_back([&, i]() {
            this_thread::sleep_for(chrono::milliseconds(50 * (i + 1)));
            cout << "  worker " << i << " done\n";
            latch.countDown();
        });
    }

    cout << "  main waiting...\n";
    latch.await();
    cout << "  main released — all workers finished\n";

    for (auto& t : workers) {
        t.join();
    }
    return 0;
}

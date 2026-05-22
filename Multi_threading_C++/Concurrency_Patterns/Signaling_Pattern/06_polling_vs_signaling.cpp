/**
 * 06 — Polling vs signaling: same "wait for ready" problem, two approaches.
 * Signaling version is efficient; polling burns CPU (demo with iteration cap).
 */
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

namespace signaling_demo {

atomic<bool> ready{false};
atomic<int> poll_iterations{0};

constexpr int kMaxPollIterations = 5'000'000;

void slow_worker() {
    this_thread::sleep_for(chrono::milliseconds(500));
    ready.store(true, memory_order_release);
}

void demo_polling() {
    ready.store(false);
    poll_iterations.store(0);

    thread t(slow_worker);
    auto start = chrono::steady_clock::now();

    while (!ready.load(memory_order_acquire)) {
        ++poll_iterations;
        if (poll_iterations.load() >= kMaxPollIterations) {
            cout << "[polling] capped at " << kMaxPollIterations
                 << " spins (would burn CPU until ready)\n";
            break;
        }
    }

    auto ms = chrono::duration_cast<chrono::milliseconds>(
                  chrono::steady_clock::now() - start)
                  .count();
    t.join();
    cout << "[polling] iterations (approx CPU spins): "
         << poll_iterations.load() << ", elapsed ms: " << ms << "\n\n";
}

void demo_signaling() {
    mutex mtx;
    condition_variable cv;
    bool done = false;

    thread t([&]() {
        this_thread::sleep_for(chrono::milliseconds(500));
        {
            lock_guard<mutex> lock(mtx);
            done = true;
        }
        cv.notify_one();
    });

    auto start = chrono::steady_clock::now();
    {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [&] { return done; });
    }
    auto ms = chrono::duration_cast<chrono::milliseconds>(
                  chrono::steady_clock::now() - start)
                  .count();
    t.join();
    cout << "[signaling] thread slept until notify, ~0 busy spins, "
            "elapsed ms: "
         << ms << "\n";
}

}  // namespace signaling_demo

int main() {
    using namespace signaling_demo;

    cout << "=== Polling (busy-wait) ===\n";
    demo_polling();

    cout << "=== Signaling (condition_variable) ===\n";
    demo_signaling();

    cout << "\nTakeaway: use signaling (cv/semaphore/future), not polling.\n";
    return 0;
}

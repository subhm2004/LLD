/**
 * 06 — Side-by-side: deadlock vs livelock vs starvation (concepts + mini demos).
 */
#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

void explain() {
    cout << "====== Deadlock vs Livelock vs Starvation ======\n\n";

    cout << "| | Deadlock | Livelock | Starvation |\n";
    cout << "|--|----------|----------|------------|\n";
    cout << "| State | Blocked on lock | Running, retrying | Waiting forever |\n";
    cout << "| CPU | Low | High | Medium |\n";
    cout << "| Progress | None | None useful | Unfair slow |\n";
    cout << "| Fix | Lock order, scoped_lock | Random backoff | Fair queue |\n\n";
}

void mini_deadlock_note() {
    cout << "[Deadlock] Threads block on mutex — program hangs.\n";
    cout << "  See: ../Deadlock/02_opposite_order_deadlock.cpp\n\n";
}

void mini_livelock_note() {
    cout << "[Livelock] try_lock + instant retry — many attempts, may never win.\n";
    cout << "  See: 02_polite_try_lock_livelock.cpp in this folder\n\n";
}

void mini_starvation_demo() {
    cout << "[Starvation] High-priority thread keeps re-locking;\n";
    cout << "  low-priority thread rarely runs.\n";

    mutex m;
    atomic<int> low_count{0};
    atomic<bool> stop{false};

    thread high([&]() {
        for (int i = 0; i < 200; ++i) {
            lock_guard<mutex> lk(m);
            this_thread::sleep_for(chrono::microseconds(10));
        }
        stop.store(true);
    });

    thread low([&]() {
        while (!stop.load()) {
            if (m.try_lock()) {
                low_count.fetch_add(1);
                m.unlock();
            }
            this_thread::yield();
        }
    });

    high.join();
    low.join();
    cout << "  Low-priority got lock only " << low_count.load()
         << " times (starved relative to high).\n\n";
}

int main() {
    explain();
    mini_deadlock_note();
    mini_livelock_note();
    mini_starvation_demo();
    return 0;
}

/**
 * 03 — Fix: random backoff between retries — breaks synchronized retry cycle.
 */
#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>

using namespace std;

mutex mtxA, mtxB;
atomic<int> total_attempts{0};
atomic<bool> success{false};

bool try_both_locks() {
    if (!mtxA.try_lock()) {
        return false;
    }
    if (!mtxB.try_lock()) {
        mtxA.unlock();
        return false;
    }
    mtxB.unlock();
    mtxA.unlock();
    return true;
}

void worker_with_backoff(int id) {
    mt19937 rng(static_cast<unsigned>(id * 9973 + 42));
    uniform_int_distribution<int> dist(5, 80);

    for (int i = 0; i < 30; ++i) {
        ++total_attempts;
        if (try_both_locks()) {
            success.store(true);
            cout << "[T" << id << "] won on attempt " << (i + 1) << "\n";
            return;
        }
        int ms = dist(rng);
        this_thread::sleep_for(chrono::milliseconds(ms));
    }
}

int main() {
    cout << "====== Random Backoff Fix ======\n\n";

    thread t1(worker_with_backoff, 1);
    thread t2(worker_with_backoff, 2);
    t1.join();
    t2.join();

    cout << "\nSuccess: " << (success.load() ? "yes" : "no") << "\n";
    cout << "Total attempts: " << total_attempts.load() << "\n";
    cout << "Random delay desynchronizes threads — progress possible.\n";
    return 0;
}

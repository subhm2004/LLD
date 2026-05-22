/**
 * 02 — Polite try_lock retry without backoff → livelock-like behavior (capped attempts).
 */
#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

mutex mtxA, mtxB;
atomic<int> total_attempts{0};
constexpr int kMaxAttempts = 50;

bool try_both_locks(int id) {
    if (!mtxA.try_lock()) {
        return false;
    }
    if (!mtxB.try_lock()) {
        mtxA.unlock();
        cout << "[T" << id << "] polite: released A, could not get B\n";
        return false;
    }
    cout << "[T" << id << "] SUCCESS — got A and B\n";
    mtxB.unlock();
    mtxA.unlock();
    return true;
}

void polite_worker(int id) {
    for (int i = 0; i < kMaxAttempts; ++i) {
        ++total_attempts;
        if (try_both_locks(id)) {
            return;
        }
        // NO backoff — immediate retry → livelock pattern
        this_thread::yield();
    }
    cout << "[T" << id << "] gave up after " << kMaxAttempts << " tries\n";
}

int main() {
    cout << "====== Polite try_lock (no backoff) ======\n";
    cout << "Both threads retry fast — high attempts, may fail to finish.\n\n";

    thread t1(polite_worker, 1);
    thread t2(polite_worker, 2);
    t1.join();
    t2.join();

    cout << "\nTotal attempts (both threads): " << total_attempts.load() << "\n";
    cout << "Without backoff, threads collide often (retry storm under contention).\n";
    cout << "Run again — sometimes high attempts before success. Compare demo 03.\n";
    return 0;
}

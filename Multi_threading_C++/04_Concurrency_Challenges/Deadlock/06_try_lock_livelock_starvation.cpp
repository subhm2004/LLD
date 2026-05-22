/**
 * 06 — try_lock + retry (avoid blocking deadlock); livelock vs starvation notes.
 */
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

mutex mtxA, mtxB;

void try_worker(int id) {
    for (int attempt = 1; attempt <= 4; ++attempt) {
        if (mtxA.try_lock()) {
            if (mtxB.try_lock()) {
                cout << "[T" << id << "] attempt " << attempt << ": got A and B\n";
                this_thread::sleep_for(chrono::milliseconds(40));
                mtxB.unlock();
                mtxA.unlock();
                return;
            }
            mtxA.unlock();
            cout << "[T" << id << "] attempt " << attempt << ": released A, retry (no partial hold)\n";
        } else {
            cout << "[T" << id << "] attempt " << attempt << ": A busy, retry\n";
        }
        this_thread::sleep_for(chrono::milliseconds(id * 40));
    }
    cout << "[T" << id << "] gave up after retries\n";
}

int main() {
    cout << "====== Fix 4: try_lock + Retry ======\n\n";

    thread t1(try_worker, 1);
    thread t2(try_worker, 2);
    t1.join();
    t2.join();

    cout << "\n--- Deadlock vs Livelock vs Starvation ---\n";
    cout << "Deadlock:  threads blocked forever, no progress.\n";
    cout << "Livelock:  threads active (retry) but no useful progress.\n";
    cout << "Starvation: one thread never gets resource (unfair scheduling).\n";
    cout << "Fix livelock: random backoff; fix starvation: fair mutex / queue.\n";
    return 0;
}

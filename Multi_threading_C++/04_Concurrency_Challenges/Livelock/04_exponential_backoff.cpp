/**
 * 04 — Exponential backoff: wait 2^attempt ms (capped) — network-style fix.
 */
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

mutex resource_mtx;

void worker(int id) {
    for (int attempt = 0; attempt < 8; ++attempt) {
        if (resource_mtx.try_lock()) {
            cout << "[T" << id << "] acquired lock on attempt " << (attempt + 1) << "\n";
            this_thread::sleep_for(chrono::milliseconds(30));
            resource_mtx.unlock();
            return;
        }
        int ms = min(200, 1 << attempt);
        cout << "[T" << id << "] attempt " << (attempt + 1)
             << " failed, backoff " << ms << "ms\n";
        this_thread::sleep_for(chrono::milliseconds(ms));
    }
    cout << "[T" << id << "] max attempts reached\n";
}

int main() {
    cout << "====== Exponential Backoff ======\n\n";

    thread t1(worker, 1);
    thread t2(worker, 2);
    t1.join();
    t2.join();

    cout << "\nBackoff grows: 1,2,4,8... ms — reduces collision under load.\n";
    return 0;
}

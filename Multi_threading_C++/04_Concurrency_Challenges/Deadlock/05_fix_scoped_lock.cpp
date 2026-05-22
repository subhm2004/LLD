/**
 * 05 — Fix: C++17 scoped_lock — one line, deadlock-safe multi-mutex.
 */
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

mutex mtxA, mtxB;

void worker(int id) {
    scoped_lock lock(mtxA, mtxB);
    cout << "[T" << id << "] scoped_lock holds A and B\n";
    this_thread::sleep_for(chrono::milliseconds(80));
}

int main() {
    cout << "====== Fix 3: scoped_lock (C++17) ======\n\n";

    thread t1(worker, 1);
    thread t2(worker, 2);
    t1.join();
    t2.join();

    cout << "\nRecommended in modern C++ for 2+ mutexes.\n";
    return 0;
}

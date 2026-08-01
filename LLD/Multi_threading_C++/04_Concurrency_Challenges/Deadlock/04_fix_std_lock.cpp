/**
 * 04 — Fix: std::lock — both mutexes acquired atomically (breaks hold-and-wait).
 */
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

mutex mtxA, mtxB;

void worker(int id) {
    unique_lock<mutex> la(mtxA, defer_lock);
    unique_lock<mutex> lb(mtxB, defer_lock);
    lock(la, lb);
    cout << "[T" << id << "] std::lock acquired A and B together\n";
    this_thread::sleep_for(chrono::milliseconds(80));
}

int main() {
    cout << "====== Fix 2: std::lock() ======\n\n";

    thread t1(worker, 1);
    thread t2(worker, 2);
    t1.join();
    t2.join();

    cout << "\nNo partial hold — hold-and-wait condition avoided.\n";
    return 0;
}

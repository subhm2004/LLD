/**
 * 03 — Fix: always acquire mtxA before mtxB (breaks circular wait).
 */
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

mutex mtxA, mtxB;

void worker(int id) {
    mtxA.lock();
    cout << "[T" << id << "] locked A\n";
    this_thread::sleep_for(chrono::milliseconds(80));
    mtxB.lock();
    cout << "[T" << id << "] locked B — same order, no deadlock\n";
    mtxB.unlock();
    mtxA.unlock();
}

int main() {
    cout << "====== Fix 1: Same Lock Order (A then B) ======\n\n";

    thread t1(worker, 1);
    thread t2(worker, 2);
    t1.join();
    t2.join();

    cout << "\nCircular wait broken — both use A -> B order.\n";
    return 0;
}

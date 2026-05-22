/**
 * 02 — Opposite lock order → circular wait (detected via try_lock_for, no infinite hang).
 */
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

timed_mutex mtxA, mtxB;

void thread1() {
    mtxA.lock();
    cout << "[T1] locked mtxA, sleeping 100ms...\n";
    this_thread::sleep_for(chrono::milliseconds(100));

    if (mtxB.try_lock_for(chrono::milliseconds(200))) {
        cout << "[T1] got mtxB (unexpected in deadlock demo)\n";
        mtxB.unlock();
    } else {
        cout << "[T1] BLOCKED on mtxB — circular wait forming!\n";
    }
    mtxA.unlock();
}

void thread2() {
    mtxB.lock();
    cout << "[T2] locked mtxB, sleeping 100ms...\n";
    this_thread::sleep_for(chrono::milliseconds(100));

    if (mtxA.try_lock_for(chrono::milliseconds(200))) {
        cout << "[T2] got mtxA (unexpected in deadlock demo)\n";
        mtxA.unlock();
    } else {
        cout << "[T2] BLOCKED on mtxA — circular wait forming!\n";
    }
    mtxB.unlock();
}

int main() {
    cout << "====== Opposite Order Deadlock (timed detection) ======\n";
    cout << "T1: A then B | T2: B then A\n\n";

    thread t1(thread1);
    thread t2(thread2);
    t1.join();
    t2.join();

    cout << "\nBoth threads finished (try_lock_for avoided forever hang).\n";
    cout << "With blocking .lock(), program would hang forever.\n";
    return 0;
}

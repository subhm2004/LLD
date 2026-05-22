/**
 * 07 — OPTIONAL: real infinite deadlock (DO NOT run in run_all.sh).
 * Compile manually only to feel the hang:
 *   g++ -std=c++17 -pthread 07_optional_infinite_deadlock.cpp -o bin/07_infinite
 *
 * Uncomment threads in main to reproduce — Ctrl+C to stop.
 */
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

mutex mtxA, mtxB;

void deadlock_t1() {
    mtxA.lock();
    this_thread::sleep_for(chrono::milliseconds(100));
    mtxB.lock();
    mtxB.unlock();
    mtxA.unlock();
}

void deadlock_t2() {
    mtxB.lock();
    this_thread::sleep_for(chrono::milliseconds(100));
    mtxA.lock();
    mtxA.unlock();
    mtxB.unlock();
}

int main() {
    cout << "====== OPTIONAL Infinite Deadlock ======\n";
    cout << "Uncomment thread lines below to hang forever.\n\n";

    // thread t1(deadlock_t1);
    // thread t2(deadlock_t2);
    // t1.join();
    // t2.join();

    cout << "Skipped (safe). Uncomment to demo real deadlock.\n";
    return 0;
}

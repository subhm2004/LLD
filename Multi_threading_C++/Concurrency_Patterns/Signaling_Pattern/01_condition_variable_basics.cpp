/**
 * 01 — Signaling basics: worker signals main when initialization completes.
 * C++17: thread, mutex, condition_variable
 */
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>

using namespace std;

namespace signaling_demo {

mutex mtx;
condition_variable cv;
bool initialized = false;

void worker() {
    cout << "[worker] loading config (simulated 2s)...\n";
    this_thread::sleep_for(chrono::seconds(2));

    {
        lock_guard<mutex> lock(mtx);
        initialized = true;
    }
    cv.notify_one();  // signal: ready to proceed

    cout << "[worker] serving requests...\n";
}

}  // namespace signaling_demo

int main() {
    using namespace signaling_demo;

    cout << "[main] starting worker, will wait for signal...\n";

    thread t(worker);

    {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [] { return initialized; });
    }

    cout << "[main] received signal — system initialized, continuing.\n";

    t.join();
    cout << "Done.\n";
    return 0;
}

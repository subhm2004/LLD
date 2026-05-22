/**
 * 02 — Same problem with semaphores: second waits for first, third waits for second.
 */
#include <iostream>
#include <thread>

#include "../common/CountingSemaphore.h"

using namespace std;
using namespace interview_common;

int main() {
    cout << "====== Print in Order — Semaphore chain ======\n";

    CountingSemaphore first_done(0);
    CountingSemaphore second_done(0);

    thread t1([&]() {
        cout << "first";
        first_done.release();
    });
    thread t2([&]() {
        first_done.acquire();
        cout << "second";
        second_done.release();
    });
    thread t3([&]() {
        second_done.acquire();
        cout << "third";
    });

    t1.join();
    t2.join();
    t3.join();

    cout << "\n";
    return 0;
}

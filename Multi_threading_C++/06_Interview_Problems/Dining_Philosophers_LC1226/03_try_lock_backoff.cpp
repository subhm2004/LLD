/**
 * 03 — try_lock + backoff (non-blocking retry).
 */
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "DiningPhilosophers.h"

using namespace std;
using namespace dining_philosophers;

int main() {
    cout << "====== Dining Philosophers — try_lock + backoff ======\n";

    DiningPhilosophers table;
    mutex print_mtx;

    vector<thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&, i]() {
            table.wantsToEatTryLock(
                i, [&]() { lock_guard<mutex> l(print_mtx); cout << "P" << i << " L\n"; },
                [&]() { lock_guard<mutex> l(print_mtx); cout << "P" << i << " R\n"; },
                [&]() { lock_guard<mutex> l(print_mtx); cout << "P" << i << " EAT\n"; },
                [&]() { lock_guard<mutex> l(print_mtx); cout << "P" << i << " putR\n"; },
                [&]() { lock_guard<mutex> l(print_mtx); cout << "P" << i << " putL\n"; });
        });
    }
    for (auto& t : threads) {
        t.join();
    }
    cout << "Done.\n";
    return 0;
}

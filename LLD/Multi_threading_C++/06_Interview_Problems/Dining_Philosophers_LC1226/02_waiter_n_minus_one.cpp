/**
 * 02 — Waiter solution: only N-1 = 4 philosophers hold forks at once.
 */
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "DiningPhilosophers.h"

using namespace std;
using namespace dining_philosophers;

int main() {
    cout << "====== Dining Philosophers — Waiter (4 seats) ======\n";

    DiningPhilosophers table;
    mutex print_mtx;

    auto runPhilosopher = [&](int id) {
        table.wantsToEatWaiter(
            id, [&]() { lock_guard<mutex> l(print_mtx); cout << "P" << id << " L\n"; },
            [&]() { lock_guard<mutex> l(print_mtx); cout << "P" << id << " R\n"; },
            [&]() { lock_guard<mutex> l(print_mtx); cout << "P" << id << " EAT\n"; },
            [&]() { lock_guard<mutex> l(print_mtx); cout << "P" << id << " putR\n"; },
            [&]() { lock_guard<mutex> l(print_mtx); cout << "P" << id << " putL\n"; });
    };

    vector<thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(runPhilosopher, i);
    }
    for (auto& t : threads) {
        t.join();
    }
    cout << "Done — no deadlock with N-1 table seats.\n";
    return 0;
}

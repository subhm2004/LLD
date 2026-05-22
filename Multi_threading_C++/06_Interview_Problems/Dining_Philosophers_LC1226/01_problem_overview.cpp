/**
 * 01 — Dining Philosophers (LC 1226) overview + ordered-forks solution.
 */
#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#include "DiningPhilosophers.h"

using namespace std;
using namespace dining_philosophers;

int main() {
    cout << "====== Dining Philosophers LC 1226 ======\n";
    cout << "5 philosophers, 5 forks, wantsToEat() = think + eat once per call\n\n";

    DiningPhilosophers table;
    mutex print_mtx;
    atomic<int> eat_count{0};

    auto runPhilosopher = [&](int id) {
        for (int round = 0; round < 2; ++round) {
            table.wantsToEat(
                id,
                [&]() {
                    lock_guard<mutex> lock(print_mtx);
                    cout << "P" << id << " pickLeft\n";
                },
                [&]() {
                    lock_guard<mutex> lock(print_mtx);
                    cout << "P" << id << " pickRight\n";
                },
                [&]() {
                    lock_guard<mutex> lock(print_mtx);
                    cout << "P" << id << " EAT\n";
                    eat_count++;
                },
                [&]() {
                    lock_guard<mutex> lock(print_mtx);
                    cout << "P" << id << " putRight\n";
                },
                [&]() {
                    lock_guard<mutex> lock(print_mtx);
                    cout << "P" << id << " putLeft\n";
                });
        }
    };

    vector<thread> threads;
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(runPhilosopher, i);
    }
    for (auto& t : threads) {
        t.join();
    }

    cout << "\nTotal eat() calls: " << eat_count.load() << " (expected 10)\n";
    cout << "Fix: ordered fork locking breaks circular wait.\n";
    return 0;
}

/**
 * 01 — Cyclic barrier: N workers finish phase k, all wait, then phase k+1.
 */
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

#include "Barrier.h"

using namespace std;
using namespace barrier_sync;

int main() {
    constexpr int kWorkers = 4;
    constexpr int kPhases = 3;

    cout << "====== Barrier (" << kWorkers << " workers, " << kPhases << " phases) ======\n";

    Barrier barrier(kWorkers);

    auto worker = [&](int id) {
        for (int phase = 1; phase <= kPhases; ++phase) {
            cout << "  worker " << id << " finished phase " << phase << "\n";
            this_thread::sleep_for(chrono::milliseconds(10 * id));
            barrier.arrive_and_wait();
            cout << "  worker " << id << " passed barrier after phase " << phase << "\n";
        }
    };

    vector<thread> threads;
    for (int i = 0; i < kWorkers; ++i) {
        threads.emplace_back(worker, i);
    }
    for (auto& t : threads) {
        t.join();
    }

    cout << "\nAll workers synchronized per phase before continuing.\n";
    return 0;
}

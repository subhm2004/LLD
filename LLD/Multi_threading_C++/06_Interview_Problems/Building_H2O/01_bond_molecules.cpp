/**
 * 01 — Building H2O (LeetCode 1117).
 * Random hydrogen/oxygen threads; output must be valid H2O sequences.
 */
#include <iostream>
#include <random>
#include <thread>
#include <vector>

#include "BuildingH2O.h"

using namespace std;
using namespace building_h2o;

int main() {
    constexpr int kMolecules = 4;
    cout << "====== Building H2O (LC 1117, molecules=" << kMolecules << ") ======\n";

    BuildingH2O builder;
    mt19937 rng(42);

    vector<thread> threads;
    threads.reserve(kMolecules * 3);

    for (int m = 0; m < kMolecules; ++m) {
        threads.emplace_back([&]() {
            this_thread::sleep_for(chrono::microseconds(rng() % 50));
            builder.hydrogen([]() { cout << 'H'; });
        });
        threads.emplace_back([&]() {
            this_thread::sleep_for(chrono::microseconds(rng() % 50));
            builder.hydrogen([]() { cout << 'H'; });
        });
        threads.emplace_back([&]() {
            this_thread::sleep_for(chrono::microseconds(rng() % 50));
            builder.oxygen([]() { cout << 'O'; });
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    cout << "\nEach H2O group: HH then O (order of thread arrival may vary within group).\n";
    cout << "Total chars: " << (kMolecules * 3) << " (all H then O per molecule when grouped).\n";
    return 0;
}

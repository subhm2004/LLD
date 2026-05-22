/**
 * 05 — Corridor analogy: two threads "step aside" same direction (livelock) vs different (fix).
 */
#include <atomic>
#include <chrono>
#include <iostream>
#include <random>
#include <thread>

using namespace std;

atomic<int> position1{0};
atomic<int> position2{0};
atomic<int> passes{0};

void corridor_livelock(bool use_random_side) {
    mt19937 rng(random_device{}());
    uniform_int_distribution<int> coin(0, 1);

    for (int round = 0; round < 20; ++round) {
        int move1 = use_random_side ? coin(rng) : 0;
        int move2 = use_random_side ? coin(rng) : 0;

        if (!use_random_side) {
            move1 = 0;
            move2 = 0;
        }

        position1.store(move1);
        position2.store(move2);

        if (move1 != move2) {
            passes.fetch_add(1);
            cout << "[pass] T1->" << move1 << " T2->" << move2 << " (someone passed)\n";
        } else {
            cout << "[stuck] both moved same side " << move1 << " (livelock step)\n";
        }
        this_thread::sleep_for(chrono::milliseconds(30));
    }
}

int main() {
    cout << "====== Corridor Simulation ======\n\n";

    cout << "--- Phase 1: Always same side (livelock) ---\n";
    passes.store(0);
    thread a([] { corridor_livelock(false); });
    a.join();
    cout << "Passes in 20 rounds: " << passes.load() << " (expect 0)\n\n";

    cout << "--- Phase 2: Random side (fix) ---\n";
    passes.store(0);
    thread b([] { corridor_livelock(true); });
    b.join();
    cout << "Passes in 20 rounds: " << passes.load() << " (expect many)\n";

    return 0;
}

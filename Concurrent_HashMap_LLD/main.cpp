// ============================================================================
//  main.cpp  —  Concurrent HashMap ka demo driver (CLIENT)
// ----------------------------------------------------------------------------
//  Demos:
//    1. Basic put/get  — dono strategies (striped + coarse) ek hi interface se
//    2. Concurrent puts (striped) — 4 threads, alag keys -> 400 entries
//    3. Same-key contention — 8 threads ek hi "hot-key" pe (last-writer-wins)
//    4. Coarse vs Striped benchmark — 8 threads x 500 puts, time compare
//    5. Concurrent reads — 3 readers ek saath (thread-safety verify)
//
//  ⭐ STRATEGY PATTERN yahan LIVE dikhta hai: demo_basic() aur benchmark_puts()
//     `IConcurrentMap&` lete hain — dono me Coarse aur Striped, dono pass hote
//     hain. Client code ek hi hai, andar ka locking algorithm alag.
//
//  📌 Includes targeted hain (bits/stdc++.h nahi) — portable + saaf.
// ============================================================================
#include <atomic>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "core/CoarseGrainedHashMap.h"
#include "core/StripedHashMap.h"

using namespace std;
using namespace concurrent_hashmap_lld;

// ---- Demo 1: basic ops — kisi bhi IConcurrentMap pe chalta hai (Strategy) ----
void demo_basic(IConcurrentMap& map) {
    cout << "\n--- Basic put/get [" << map.strategyName() << "] ---\n";
    map.put("user:1", "Alice");
    map.put("user:2", "Bob");

    auto v1 = map.get("user:1");  // hit -> optional me "Alice"
    auto v2 = map.get("user:99"); // miss -> nullopt
    cout << "user:1 => " << (v1 ? *v1 : string("MISS")) << "\n";
    cout << "user:99 => " << (v2 ? *v2 : string("MISS")) << "\n";
    cout << "size=" << map.size() << "\n";
}

// ---- Demo 2: 4 threads, ALAG keys -> striping se parallel chalte hain --------
void demo_concurrent_puts_striped() {
    cout << "\n=== Concurrent puts (striped — different keys) ===\n";
    StripedHashMap map(16);

    vector<thread> workers;
    for (int t = 0; t < 4; ++t) {
        // Har thread apni unique keys ("t0-k0", "t1-k0"...) daalta hai. Keys alag
        // hain to zyadatar alag stripes me girti hain -> threads parallel chalte.
        workers.emplace_back([&map, t]() {
            for (int i = 0; i < 100; ++i) {
                string key = "t" + to_string(t) + "-k" + to_string(i);
                map.put(key, "v" + to_string(i));
            }
        });
    }
    for (auto& th : workers) {
        th.join();
    }
    // 4 threads x 100 unique keys = 400 entries. Agar thread-safety toot-ti to
    // ye number galat aata (lost updates). 400 = sahi.
    cout << "final size=" << map.size() << " (expect 400)\n";
    map.statistics().print();
}

// ---- Demo 3: SAME key pe 8 threads — "last writer wins" -------------------
void demo_same_key_contention() {
    cout << "\n=== Same key contention (striped) ===\n";
    StripedHashMap map(8);
    atomic<int> success_puts{0};

    vector<thread> workers;
    for (int t = 0; t < 8; ++t) {
        // Sab threads ek hi "hot-key" pe likh rahe hain -> sab SAME stripe (same
        // lock) pe serialize honge (striping yahan help nahi karta — ek hi key).
        // Aakhir me jo THREAD aakhri me likhega, uski value rahegi (last-writer-wins).
        workers.emplace_back([&]() {
            for (int i = 0; i < 50; ++i) {
                map.put("hot-key", "v" + to_string(i));
                success_puts.fetch_add(1);
            }
        });
    }
    for (auto& th : workers) {
        th.join();
    }
    auto v = map.get("hot-key");
    // size = 1 (ek hi key), par puts issued = 8x50 = 400 (sab writes hue, ek hi key pe).
    cout << "hot-key => " << (v ? *v : string("MISS")) << "\n";
    cout << "puts issued=" << success_puts.load() << " size=" << map.size() << "\n";
}

// ---- Benchmark helper: N threads x M puts, time naapo --------------------
void benchmark_puts(IConcurrentMap& map, const string& label, int threads, int ops_each) {
    auto start = chrono::steady_clock::now();

    vector<thread> workers;
    for (int t = 0; t < threads; ++t) {
        workers.emplace_back([&map, t, ops_each]() {
            for (int i = 0; i < ops_each; ++i) {
                map.put("bench-" + to_string(t) + "-" + to_string(i), "x");
            }
        });
    }
    for (auto& th : workers) {
        th.join();
    }

    auto ms = chrono::duration_cast<chrono::milliseconds>(
                  chrono::steady_clock::now() - start)
                  .count();
    cout << label << ": " << ms << " ms, size=" << map.size() << "\n";
}

// ---- Demo 4: coarse vs striped — same load, time compare ------------------
void demo_coarse_vs_striped() {
    cout << "\n=== Coarse vs Striped (8 threads x 500 puts) ===\n";
    const int kThreads = 8;
    const int kOps = 500;

    // Coarse: sab threads ek hi lock ke liye ladte hain -> zyada wait.
    CoarseGrainedHashMap coarse;
    benchmark_puts(coarse, "Coarse-grained", kThreads, kOps);

    // Striped: 32 stripes -> alag keys alag locks -> kam wait, aksar tez.
    StripedHashMap striped(32);
    benchmark_puts(striped, "Lock striping (32 stripes)", kThreads, kOps);

    cout << "(Striped often faster under multi-key parallel load — machine dependent)\n";
}

// ---- Demo 5: concurrent reads — 3 readers ek saath -----------------------
void demo_concurrent_reads() {
    cout << "\n=== Concurrent reads (striped) ===\n";
    StripedHashMap map(16);
    map.put("config", "production");

    // ⭐ std::cout KHUD thread-safe NAHI hai concurrent formatted output ke liye!
    //    Do threads ek saath `cout << ...` karein to stream ki internal state
    //    (width/format flags) pe race hoti hai (ThreadSanitizer ise pakadta hai).
    //    Isliye jab thread ke ANDAR se print karein, ek mutex se guard karo.
    //    (Map thread-safe hai; ye race hoti to sirf LOGGING ki, agar guard na hota.)
    mutex coutMutex;

    auto reader = [&map, &coutMutex](int id) {
        for (int i = 0; i < 20; ++i) {
            auto v = map.get("config");
            (void)v; // value use nahi kar rahe, bas read stress
            this_thread::sleep_for(chrono::milliseconds(2));
        }
        {
            lock_guard<mutex> lock(coutMutex); // cout ko ek waqt me ek thread hi use kare
            cout << "[reader " << id << "] done\n";
        }
    };

    // Teen reader threads same key padh rahe hain — map thread-safe hai.
    // (Note: get() bhi lock leta hai kyunki wo stats_ update karta hai -> reads
    //  bhi exclusive hain is design me. shared_mutex se parallel reads possible —
    //  wo ek extension hai, requirements me listed.)
    thread r1(reader, 1);
    thread r2(reader, 2);
    thread r3(reader, 3);
    r1.join();
    r2.join();
    r3.join();
    map.statistics().print();
}

int main() {
    cout << "====== Concurrent HashMap LLD (C++17) ======\n";

    // Ek hi demo_basic() dono strategies pe (Strategy pattern — polymorphic call)
    StripedHashMap striped(16);
    demo_basic(striped);

    CoarseGrainedHashMap coarse;
    demo_basic(coarse);

    demo_concurrent_puts_striped();
    demo_same_key_contention();
    demo_coarse_vs_striped();
    demo_concurrent_reads();

    cout << "\nDemo complete.\n";
    return 0;
}

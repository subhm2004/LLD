#include <bits/stdc++.h>

#include "core/CoarseGrainedHashMap.h"
#include "core/StripedHashMap.h"

using namespace std;
using namespace concurrent_hashmap_lld;

void demo_basic(IConcurrentMap& map) {
    cout << "\n--- Basic put/get [" << map.strategyName() << "] ---\n";
    map.put("user:1", "Alice");
    map.put("user:2", "Bob");

    auto v1 = map.get("user:1");
    auto v2 = map.get("user:99");
    cout << "user:1 => " << (v1 ? *v1 : string("MISS")) << "\n";
    cout << "user:99 => " << (v2 ? *v2 : string("MISS")) << "\n";
    cout << "size=" << map.size() << "\n";
}

void demo_concurrent_puts_striped() {
    cout << "\n=== Concurrent puts (striped — different keys) ===\n";
    StripedHashMap map(16);

    vector<thread> workers;
    for (int t = 0; t < 4; ++t) {
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
    cout << "final size=" << map.size() << " (expect 400)\n";
    map.statistics().print();
}

void demo_same_key_contention() {
    cout << "\n=== Same key contention (striped) ===\n";
    StripedHashMap map(8);
    atomic<int> success_puts{0};

    vector<thread> workers;
    for (int t = 0; t < 8; ++t) {
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
    cout << "hot-key => " << (v ? *v : string("MISS")) << "\n";
    cout << "puts issued=" << success_puts.load() << " size=" << map.size() << "\n";
}

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

void demo_coarse_vs_striped() {
    cout << "\n=== Coarse vs Striped (8 threads x 500 puts) ===\n";
    const int kThreads = 8;
    const int kOps = 500;

    CoarseGrainedHashMap coarse;
    benchmark_puts(coarse, "Coarse-grained", kThreads, kOps);

    StripedHashMap striped(32);
    benchmark_puts(striped, "Lock striping (32 stripes)", kThreads, kOps);

    cout << "(Striped often faster under multi-key parallel load — machine dependent)\n";
}

void demo_concurrent_reads() {
    cout << "\n=== Concurrent reads (striped) ===\n";
    StripedHashMap map(16);
    map.put("config", "production");

    auto reader = [&map](int id) {
        for (int i = 0; i < 20; ++i) {
            auto v = map.get("config");
            (void)v;
            this_thread::sleep_for(chrono::milliseconds(2));
        }
        cout << "[reader " << id << "] done\n";
    };

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

#include <bits/stdc++.h>

#include "core/ThreadSafeTTLCache.h"

using namespace std;
using namespace ttl_cache_lld;

void demo_basic_ttl() {
    cout << "\n=== 1. Basic put / get / TTL expiry ===\n";
    ThreadSafeTTLCache cache(100);

    cache.put("user:1", "Alice", 2);
    auto v = cache.get("user:1");
    cout << "get user:1 => " << (v ? *v : string("MISS")) << "\n";

    cout << "sleep 2.5s for TTL expiry...\n";
    this_thread::sleep_for(chrono::milliseconds(2500));

    v = cache.get("user:1");
    cout << "get user:1 after TTL => " << (v ? *v : string("MISS (expired)")) << "\n";
    cache.printEntries();
}

void demo_concurrent_reads() {
    cout << "\n=== 2. Concurrent reads (shared_lock) ===\n";
    ThreadSafeTTLCache cache(100);
    cache.put("config", "v1.0.0", 60);

    auto reader = [&cache](int id) {
        for (int i = 0; i < 5; ++i) {
            auto v = cache.get("config");
            cout << "[reader " << id << "] " << (v ? *v : string("miss")) << "\n";
            this_thread::sleep_for(chrono::milliseconds(20));
        }
    };

    thread r1(reader, 1);
    thread r2(reader, 2);
    thread r3(reader, 3);
    r1.join();
    r2.join();
    r3.join();
}

void demo_concurrent_puts_same_key() {
    cout << "\n=== 3. Concurrent put same key (serialized writes) ===\n";
    ThreadSafeTTLCache cache(100);

    auto writer = [&cache](int id) {
        string val = "writer-" + to_string(id);
        cache.put("token", val, 30);
        cout << "[writer " << id << "] put token=" << val << "\n";
    };

    thread w1(writer, 1);
    thread w2(writer, 2);
    w1.join();
    w2.join();

    auto final_val = cache.get("token");
    cout << "final token => " << (final_val ? *final_val : string("miss")) << "\n";
}

void demo_cleanup_vs_get() {
    cout << "\n=== 4. cleanupExpired vs concurrent get ===\n";
    ThreadSafeTTLCache cache(50);
    cache.put("a", "1", 1);
    cache.put("b", "2", 1);
    cache.put("c", "3", 60);

    this_thread::sleep_for(chrono::milliseconds(1200));

    thread t_get([&]() {
        for (int i = 0; i < 10; ++i) {
            cache.get("a");
            cache.get("c");
            this_thread::sleep_for(chrono::milliseconds(10));
        }
    });

    thread t_clean([&]() {
        this_thread::sleep_for(chrono::milliseconds(50));
        int n = cache.cleanupExpired();
        cout << "[cleanup] removed " << n << " expired entries\n";
    });

    t_get.join();
    t_clean.join();
    cout << "active size=" << cache.size()
         << " total_in_map=" << cache.totalEntriesIncludingExpired() << "\n";
}

void demo_capacity_eviction() {
    cout << "\n=== 5. Max capacity eviction ===\n";
    ThreadSafeTTLCache cache(3);

    cache.put("k1", "v1", 100);
    cache.put("k2", "v2", 1);
    cache.put("k3", "v3", 100);
    this_thread::sleep_for(chrono::milliseconds(1100));
    cache.put("k4", "v4", 100);

    cache.printEntries();
    cout << "active size=" << cache.size() << " (max 3)\n";
}

int main() {
    cout << "====== Thread-Safe Cache with TTL LLD ======\n";

    demo_basic_ttl();
    demo_concurrent_reads();
    demo_concurrent_puts_same_key();
    demo_cleanup_vs_get();
    demo_capacity_eviction();

    ThreadSafeTTLCache cache(10);
    cache.put("final", "stats-demo", 10);
    cache.get("final");
    cache.get("missing");
    cache.statistics().print();

    cout << "\nDemo complete.\n";
    return 0;
}

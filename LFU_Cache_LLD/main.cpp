// ============================================================================
//  main.cpp  —  LFU Cache ka demo driver (CLIENT)
// ----------------------------------------------------------------------------
//  Teen demo chalte hain:
//    1. Basic LFU     -> frequency ke hisaab se eviction dikhata hai
//    2. Frequency track -> get se frequency badhna + lowest-freq evict
//    3. Concurrent stress -> 16 threads ek saath -> thread-safety verify
//
//  ⭐ Client ko sirf `CacheService` dikhta hai (Facade). LFUCacheCore,
//     ThreadSafeLFUCache, buckets, minFreq — kuch nahi. Bas get/put/contains.
//
//  📌 Includes targeted hain (bits/stdc++.h nahi) — requirements ki NFR4
//     "no bits/stdc++.h" follow karte hue, aur portable (GCC-only header se bacho).
// ============================================================================
#include <iostream>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "config/CacheConfig.h"
#include "core/CacheService.h"
#include "enums/CacheOperationType.h"
#include "utils/ConcurrencyHelpers.h"

using std::cout;
using std::lock_guard;
using std::mutex;
using std::nullopt;
using std::optional;
using std::string;
using std::thread;
using std::vector;
using namespace lfu_cache_lld;

static void printCppStandardBanner() {
    cout << "LFU_Cache_LLD — C++17 (__cplusplus=" << __cplusplus << ")\n";
}

// ============================================================================
//  DEMO 1 — Basic LFU: frequency-based eviction
// ============================================================================
void runBasicLfuDemo() {
    cout << "\n########## Basic LFU Demo ##########\n";
    cout << "(Evicts lowest frequency; tie -> LRU within that frequency)\n";

    const CacheConfig config(3U); // capacity 3
    CacheService<string, string> cache(config);

    // Chhota helper — ek operation chala ke uska description print karo.
    const auto log = [&cache](const CacheOperationType op, const string &key,
                              const optional<string> &value = nullopt) {
        cout << cache.executeAndDescribe(op, key, value) << '\n';
    };

    // A, B, C daale -> teeno ki frequency 1
    log(CacheOperationType::PUT, "A", "alpha");
    log(CacheOperationType::PUT, "B", "beta");
    log(CacheOperationType::PUT, "C", "gamma");

    // A ko 3 baar get -> A ki frequency 1 -> 4 (B, C abhi bhi 1)
    log(CacheOperationType::GET, "A");
    log(CacheOperationType::GET, "A");
    log(CacheOperationType::GET, "A");

    // D daala -> capacity full (3) -> lowest freq (B ya C, freq 1) evict.
    // B aur C dono freq 1 -> tie -> LRU within freq -> B (purana) evict hota hai.
    log(CacheOperationType::PUT, "D", "delta");

    log(CacheOperationType::GET, "B"); // MISS (B evict ho gaya)
    log(CacheOperationType::GET, "A"); // HIT (A high freq, safe tha)
    log(CacheOperationType::GET, "C"); // HIT
    log(CacheOperationType::GET, "D"); // HIT

    cache.printState();
    cache.printStatistics();
}

// ============================================================================
//  DEMO 2 — Frequency tracking + lowest-frequency eviction
// ============================================================================
void runFrequencyTrackingDemo() {
    cout << "\n########## Frequency Tracking Demo (structured bindings) ##########\n";

    const CacheConfig config(3U);
    CacheService<string, int> cache(config);

    // 3 API paths daale (structured bindings [path, payload] se loop)
    const vector<std::pair<string, int>> entries{
        {"api:/health", 3}, {"api:/orders", 2}, {"api:/users", 1}};
    for (const auto &[path, payload] : entries) {
        cache.put(path, payload);
    }

    // users ko 5 baar, orders ko 2 baar get -> unki frequency badhao
    for (int i = 0; i < 5; ++i) {
        (void)cache.get("api:/users");  // (void) = return jaan-boojh ke ignore
    }
    for (int i = 0; i < 2; ++i) {
        (void)cache.get("api:/orders");
    }

    // Ab frequencies: users sabse zyada, health sabse kam (sirf initial put)
    const struct {
        const char *label;
        const char *key;
    } freqChecks[] = {{"users", "api:/users"}, {"orders", "api:/orders"}, {"health", "api:/health"}};
    for (const auto &[label, key] : freqChecks) {
        cout << "freq(api:/" << label << ")  = " << cache.getFrequency(key) << '\n';
    }

    // Naya path daala -> capacity full -> LOWEST freq (health) evict hoga
    cache.put("api:/metrics", 4);

    cout << "After adding api:/metrics (capacity 3 full -> evicts lowest freq):\n";
    cout << "contains(api:/health)  = " << (cache.contains("api:/health") ? "true" : "false")
         << " (expected false)\n";
    cout << "contains(api:/users)   = " << (cache.contains("api:/users") ? "true" : "false")
         << '\n';

    cache.printStatistics();
}

// ============================================================================
//  DEMO 3 — Concurrent stress test (thread-safety verify)
// ============================================================================
void runConcurrentStressTest() {
    cout << "\n########## Concurrent Stress Test ##########\n";

    constexpr int capacity = 100;
    constexpr int numThreads = 16;
    constexpr int operationsPerThread = 500;

    const CacheConfig config(static_cast<std::size_t>(capacity));
    CacheService<int, int> cache(config);

    CyclicBarrier startBarrier(numThreads); // sab ek saath shuru
    CountDownLatch doneLatch(numThreads);    // sab khatam hone ka signal
    mutex coutMutex;

    vector<thread> workers;
    workers.reserve(static_cast<std::size_t>(numThreads));

    for (int threadId = 0; threadId < numThreads; ++threadId) {
        workers.emplace_back([&cache, &startBarrier, &doneLatch, threadId]() {
            startBarrier.arriveAndWait(); // sab yahan ruk ke ek saath chhootenge

            for (int op = 0; op < operationsPerThread; ++op) {
                const int key = (threadId * 13 + op) % (capacity * 3);
                if (op % 4 == 0) {
                    cache.put(key, key * 10 + threadId); // 25% puts
                } else {
                    (void)cache.get(key);                // 75% gets
                }
            }
            doneLatch.countDown(); // mera kaam khatam
        });
    }

    for (thread &worker : workers) {
        worker.join(); // saare threads finish hone do
    }
    doneLatch.await();

    // ⭐ Agar cache thread-safe nahi hota, to itne parallel get/put me crash ya
    //    corrupt state hota. Saaf output = locking sahi kaam kar rahi hai.
    {
        lock_guard<mutex> lock(coutMutex);
        cout << "Threads finished: " << numThreads << '\n';
        cache.printState();
        cache.printStatistics();
    }
}

int main() {
    std::ios::sync_with_stdio(false); // I/O thoda tez (C stdio se de-sync)
    std::cin.tie(nullptr);

    printCppStandardBanner();

    runBasicLfuDemo();
    runFrequencyTrackingDemo();
    runConcurrentStressTest();

    cout << "\nAll LFU demos completed successfully.\n";
    return 0;
}

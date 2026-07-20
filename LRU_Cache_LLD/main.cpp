// ============================================================================
//  main.cpp  —  LRU Cache ka demo driver (CLIENT)
// ----------------------------------------------------------------------------
//  Teen demo chalte hain:
//    1. Basic LRU        -> recency ke hisaab se eviction dikhata hai
//    2. Deterministic    -> 2 threads controlled writes (predictable-ish)
//    3. Concurrent stress -> 16 threads ek saath -> thread-safety verify
//
//  ⭐ Client ko sirf `CacheService` dikhta hai (Facade). LRUCacheCore,
//     ThreadSafeLRUCache, list, splice — kuch nahi. Bas get/put/contains.
//
//  📌 Includes targeted hain (bits/stdc++.h nahi) — portable aur saaf.
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
using std::pair;
using std::string;
using std::thread;
using std::vector;
using namespace lru_cache_lld;

// ============================================================================
//  DEMO 1 — Basic LRU: recency-based eviction
// ============================================================================
void runBasicLruDemo() {
  cout << "\n########## Basic LRU Demo ##########\n";

  CacheConfig config(3); // capacity 3
  CacheService<string, string> cache(config);

  // Chhota helper — ek operation chala ke description print karo.
  auto log = [&](CacheOperationType op, const string &key,
                 const optional<string> &value = nullopt) {
    cout << cache.executeAndDescribe(op, key, value) << "\n";
  };

  // 3 daale -> cache full. order (MRU->LRU): user:3, user:2, user:1
  log(CacheOperationType::PUT, "user:1", "Alice");
  log(CacheOperationType::PUT, "user:2", "Bob");
  log(CacheOperationType::PUT, "user:3", "Charlie");

  // 4th daala -> LRU (user:1, sabse purana) evict hota hai
  log(CacheOperationType::PUT, "user:4", "David"); // evicts user:1

  log(CacheOperationType::GET, "user:1"); // MISS (user:1 evict ho gaya)
  log(CacheOperationType::GET, "user:2"); // HIT -> user:2 ab MRU ban gaya (promote)

  // 5th daala -> ab LRU user:3 hai (user:2 abhi promote hua tha) -> user:3 evict
  log(CacheOperationType::PUT, "user:5", "Eve"); // evicts LRU (user:3)

  log(CacheOperationType::GET, "user:3"); // MISS
  log(CacheOperationType::GET, "user:2"); // HIT

  log(CacheOperationType::REMOVE, "user:4");
  log(CacheOperationType::CONTAINS, "user:4"); // false (abhi hataya)
  log(CacheOperationType::CONTAINS, "user:2"); // true

  cache.printState();
  cache.printStatistics();
}

// ============================================================================
//  DEMO 2 — Deterministic concurrency (2 threads, controlled writes)
// ============================================================================
void runDeterministicConcurrencyDemo() {
  cout << "\n########## Deterministic Concurrency Demo ##########\n";

  CacheConfig config(2); // capacity sirf 2 -> eviction jaldi dikhega
  CacheService<string, int> cache(config);

  cache.put("A", 1);
  cache.put("B", 2);

  mutex coutMutex; // sirf print ko interleave hone se rokne ke liye

  // Har thread apni writes karta hai, phir turant get + print.
  auto worker = [&](const string &name,
                    const vector<pair<string, int>> &writes) {
    for (const auto &entry : writes) {
      cache.put(entry.first, entry.second);
      optional<int> value = cache.get(entry.first);
      lock_guard<mutex> lock(coutMutex); // print atomic rahe
      cout << name << " -> key=" << entry.first << " value=";
      if (value.has_value()) {
        cout << value.value();
      } else {
        cout << "MISS";
      }
      cout << " | size=" << cache.size() << "\n";
    }
  };

  // Do threads ek saath cache pe likhte hain -> thread-safe hona chahiye
  thread t1(worker, "T1", vector<pair<string, int>>{{"A", 10}, {"C", 30}});
  thread t2(worker, "T2", vector<pair<string, int>>{{"B", 20}, {"D", 40}});

  t1.join();
  t2.join();

  cache.printState();
  cache.printStatistics();
}

// ============================================================================
//  DEMO 3 — Concurrent stress test (thread-safety verify)
// ============================================================================
void runConcurrentStressTest() {
  cout << "\n########## Concurrent Stress Test ##########\n";

  const int capacity = 100;
  const int numThreads = 16;
  const int operationsPerThread = 500;

  CacheConfig config(capacity);
  CacheService<int, int> cache(config);

  CyclicBarrier startBarrier(numThreads); // sab ek saath shuru
  CountDownLatch doneLatch(numThreads);    // sab khatam hone ka signal
  mutex coutMutex;

  vector<thread> workers;
  workers.reserve(numThreads);

  for (int threadId = 0; threadId < numThreads; ++threadId) {
    workers.emplace_back([&, threadId]() {
      startBarrier.arriveAndWait(); // sab yahan ruk ke ek saath chhootenge

      for (int op = 0; op < operationsPerThread; ++op) {
        int key = (threadId * 17 + op) % (capacity * 3);
        if (op % 3 == 0) {
          cache.put(key, key * 10 + threadId); // ~33% puts
        } else {
          cache.get(key);                       // ~67% gets
        }
      }
      doneLatch.countDown(); // mera kaam khatam
    });
  }

  for (thread &worker : workers) {
    worker.join();
  }
  doneLatch.await();

  // ⭐ Itne parallel get/put me crash/corruption nahi = locking sahi hai.
  {
    lock_guard<mutex> lock(coutMutex);
    cout << "Threads finished: " << numThreads << "\n";
    cout << "Total operations (approx): " << (numThreads * operationsPerThread)
         << "\n";
    cache.printState();
    cache.printStatistics();
  }
}

int main() {
  std::ios::sync_with_stdio(false); // I/O thoda tez
  std::cin.tie(nullptr);

  runBasicLruDemo();
  runDeterministicConcurrencyDemo();
  runConcurrentStressTest();

  cout << "\nAll demos completed successfully.\n";
  return 0;
}

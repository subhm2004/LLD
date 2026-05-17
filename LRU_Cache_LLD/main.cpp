#include <bits/stdc++.h>

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

void runBasicLruDemo() {
  cout << "\n########## Basic LRU Demo ##########\n";

  CacheConfig config(3);
  CacheService<string, string> cache(config);

  auto log = [&](CacheOperationType op, const string &key,
                 const optional<string> &value = nullopt) {
    cout << cache.executeAndDescribe(op, key, value) << "\n";
  };

  log(CacheOperationType::PUT, "user:1", "Alice");
  log(CacheOperationType::PUT, "user:2", "Bob");
  log(CacheOperationType::PUT, "user:3", "Charlie");
  log(CacheOperationType::PUT, "user:4", "David"); // evicts user:1

  log(CacheOperationType::GET, "user:1"); // miss
  log(CacheOperationType::GET, "user:2"); // hit, promotes user:2

  log(CacheOperationType::PUT, "user:5", "Eve"); // evicts LRU (user:3)

  log(CacheOperationType::GET, "user:3"); // miss
  log(CacheOperationType::GET, "user:2"); // hit

  log(CacheOperationType::REMOVE, "user:4");
  log(CacheOperationType::CONTAINS, "user:4");
  log(CacheOperationType::CONTAINS, "user:2");

  cache.printState();
  cache.printStatistics();
}

void runConcurrentStressTest() {
  cout << "\n########## Concurrent Stress Test ##########\n";

  const int capacity = 100;
  const int numThreads = 16;
  const int operationsPerThread = 500;

  CacheConfig config(capacity);
  CacheService<int, int> cache(config);

  CyclicBarrier startBarrier(numThreads);
  CountDownLatch doneLatch(numThreads);
  mutex coutMutex;

  vector<thread> workers;
  workers.reserve(numThreads);

  for (int threadId = 0; threadId < numThreads; ++threadId) {
    workers.emplace_back([&, threadId]() {
      startBarrier.arriveAndWait();

      for (int op = 0; op < operationsPerThread; ++op) {
        int key = (threadId * 17 + op) % (capacity * 3);
        if (op % 3 == 0) {
          cache.put(key, key * 10 + threadId);
        } else {
          cache.get(key);
        }
      }

      doneLatch.countDown();
    });
  }

  for (thread &worker : workers) {
    worker.join();
  }

  doneLatch.await();

  {
    lock_guard<mutex> lock(coutMutex);
    cout << "Threads finished: " << numThreads << "\n";
    cout << "Total operations (approx): " << (numThreads * operationsPerThread)
         << "\n";
    cache.printState();
    cache.printStatistics();
  }
}

void runDeterministicConcurrencyDemo() {
  cout << "\n########## Deterministic Concurrency Demo ##########\n";

  CacheConfig config(2);
  CacheService<string, int> cache(config);

  cache.put("A", 1);
  cache.put("B", 2);

  mutex coutMutex;

  auto worker = [&](const string &name,
                    const vector<pair<string, int>> &writes) {
    for (const auto &entry : writes) {
      cache.put(entry.first, entry.second);
      optional<int> value = cache.get(entry.first);
      lock_guard<mutex> lock(coutMutex);
      cout << name << " -> key=" << entry.first << " value=";
      if (value.has_value()) {
        cout << value.value();
      } else {
        cout << "MISS";
      }
      cout << " | size=" << cache.size() << "\n";
    }
  };

  thread t1(worker, "T1", vector<pair<string, int>>{{"A", 10}, {"C", 30}});
  thread t2(worker, "T2", vector<pair<string, int>>{{"B", 20}, {"D", 40}});

  t1.join();
  t2.join();

  cache.printState();
  cache.printStatistics();
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  runBasicLruDemo();
  runDeterministicConcurrencyDemo();
  runConcurrentStressTest();

  cout << "\nAll demos completed successfully.\n";
  return 0;
}

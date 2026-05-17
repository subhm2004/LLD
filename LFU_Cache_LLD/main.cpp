#include <iostream>
#include <mutex>
#include <optional>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "config/CacheConfig.h"
#include "config/CppStandard.h"
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
    cout << "LFU_Cache_LLD — C++" << kCppStandardYear << " (__cplusplus=" << __cplusplus
         << ")\n";
}

void runBasicLfuDemo() {
    cout << "\n########## Basic LFU Demo ##########\n";
    cout << "(Evicts lowest frequency; tie -> LRU within that frequency)\n";

    const CacheConfig config(3U);
    CacheService<string, string> cache(config);

    const auto log = [&cache](const CacheOperationType op, const string &key,
                              const optional<string> &value = nullopt) {
        cout << cache.executeAndDescribe(op, key, value) << '\n';
    };

    log(CacheOperationType::PUT, "A", "alpha");
    log(CacheOperationType::PUT, "B", "beta");
    log(CacheOperationType::PUT, "C", "gamma");

    log(CacheOperationType::GET, "A");
    log(CacheOperationType::GET, "A");
    log(CacheOperationType::GET, "A");

    log(CacheOperationType::PUT, "D", "delta");

    log(CacheOperationType::GET, "B");
    log(CacheOperationType::GET, "A");
    log(CacheOperationType::GET, "C");
    log(CacheOperationType::GET, "D");

    cache.printState();
    cache.printStatistics();
}

void runFrequencyTrackingDemo() {
    cout << "\n########## Frequency Tracking Demo (structured bindings) ##########\n";

    const CacheConfig config(3U);
    CacheService<string, int> cache(config);

    const vector<std::pair<string, int>> entries{
        {"api:/health", 3}, {"api:/orders", 2}, {"api:/users", 1}};

    for (const auto &[path, payload] : entries) {
        cache.put(path, payload);
    }

    for (int i = 0; i < 5; ++i) {
        (void)cache.get("api:/users");
    }
    for (int i = 0; i < 2; ++i) {
        (void)cache.get("api:/orders");
    }

    const struct {
        const char *label;
        const char *key;
    } freqChecks[] = {{"users", "api:/users"}, {"orders", "api:/orders"}, {"health", "api:/health"}};

    for (const auto &[label, key] : freqChecks) {
        cout << "freq(api:/" << label << ")  = " << cache.getFrequency(key) << '\n';
    }

    cache.put("api:/metrics", 4);

    cout << "After adding api:/metrics (capacity 3 full -> evicts lowest freq):\n";
    cout << "contains(api:/health)  = " << (cache.contains("api:/health") ? "true" : "false")
         << " (expected false)\n";
    cout << "contains(api:/users)   = " << (cache.contains("api:/users") ? "true" : "false")
         << '\n';

    cache.printStatistics();
}

void runConcurrentStressTest() {
    cout << "\n########## Concurrent Stress Test ##########\n";

    constexpr int capacity = 100;
    constexpr int numThreads = 16;
    constexpr int operationsPerThread = 500;

    const CacheConfig config(static_cast<std::size_t>(capacity));
    CacheService<int, int> cache(config);

    CyclicBarrier startBarrier(numThreads);
    CountDownLatch doneLatch(numThreads);
    mutex coutMutex;

    vector<thread> workers;
    workers.reserve(static_cast<std::size_t>(numThreads));

    for (int threadId = 0; threadId < numThreads; ++threadId) {
        workers.emplace_back([&cache, &startBarrier, &doneLatch, threadId]() {
            startBarrier.arriveAndWait();

            for (int op = 0; op < operationsPerThread; ++op) {
                const int key = (threadId * 13 + op) % (capacity * 3);
                if (op % 4 == 0) {
                    cache.put(key, key * 10 + threadId);
                } else {
                    (void)cache.get(key);
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
        cout << "Threads finished: " << numThreads << '\n';
        cache.printState();
        cache.printStatistics();
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    printCppStandardBanner();

    runBasicLfuDemo();
    runFrequencyTrackingDemo();
    runConcurrentStressTest();

    cout << "\nAll LFU demos completed successfully.\n";
    return 0;
}

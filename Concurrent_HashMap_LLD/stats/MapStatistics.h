#ifndef CONCURRENT_HASHMAP_LLD_STATS_MAPSTATISTICS_H
#define CONCURRENT_HASHMAP_LLD_STATS_MAPSTATISTICS_H

#include <atomic>
#include <iostream>

namespace concurrent_hashmap_lld {

struct MapStatistics {
    std::atomic<long long> gets{0};
    std::atomic<long long> puts{0};
    std::atomic<long long> removes{0};
    std::atomic<long long> hits{0};
    std::atomic<long long> misses{0};

    void recordGet(bool hit) {
        gets.fetch_add(1);
        if (hit) {
            hits.fetch_add(1);
        } else {
            misses.fetch_add(1);
        }
    }

    void recordPut() { puts.fetch_add(1); }
    void recordRemove() { removes.fetch_add(1); }

    void print() const {
        std::cout << "\n=== Map Statistics ===\n";
        std::cout << "Gets    : " << gets.load() << "\n";
        std::cout << "Hits    : " << hits.load() << "\n";
        std::cout << "Misses  : " << misses.load() << "\n";
        std::cout << "Puts    : " << puts.load() << "\n";
        std::cout << "Removes : " << removes.load() << "\n";
        std::cout << "======================\n";
    }
};

}  // namespace concurrent_hashmap_lld

#endif  // CONCURRENT_HASHMAP_LLD_STATS_MAPSTATISTICS_H

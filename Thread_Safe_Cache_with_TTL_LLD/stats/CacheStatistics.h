#ifndef THREAD_SAFE_TTL_CACHE_LLD_STATS_CACHESTATISTICS_H
#define THREAD_SAFE_TTL_CACHE_LLD_STATS_CACHESTATISTICS_H

#include <atomic>
#include <iostream>

namespace ttl_cache_lld {

struct CacheStatistics {
    std::atomic<long long> hits{0};
    std::atomic<long long> misses{0};
    std::atomic<long long> puts{0};
    std::atomic<long long> expired_on_get{0};
    std::atomic<long long> lazy_removals{0};
    std::atomic<long long> cleanup_sweeps{0};

    void recordHit() { hits.fetch_add(1); }
    void recordMiss() { misses.fetch_add(1); }
    void recordPut() { puts.fetch_add(1); }
    void recordExpiredOnGet() { expired_on_get.fetch_add(1); }
    void recordLazyRemoval() { lazy_removals.fetch_add(1); }
    void recordCleanupSweep(int removed) {
        cleanup_sweeps.fetch_add(1);
        lazy_removals.fetch_add(removed);
    }

    void print() const {
        std::cout << "\n=== Cache Statistics ===\n";
        std::cout << "Hits            : " << hits.load() << "\n";
        std::cout << "Misses          : " << misses.load() << "\n";
        std::cout << "Puts            : " << puts.load() << "\n";
        std::cout << "Expired on get  : " << expired_on_get.load() << "\n";
        std::cout << "Lazy removals   : " << lazy_removals.load() << "\n";
        std::cout << "Cleanup sweeps  : " << cleanup_sweeps.load() << "\n";
        std::cout << "========================\n";
    }
};

}  // namespace ttl_cache_lld

#endif  // THREAD_SAFE_TTL_CACHE_LLD_STATS_CACHESTATISTICS_H

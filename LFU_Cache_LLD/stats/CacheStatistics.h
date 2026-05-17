#ifndef LFU_CACHE_LLD_STATS_CACHESTATISTICS_H
#define LFU_CACHE_LLD_STATS_CACHESTATISTICS_H

#include <atomic>
#include <iostream>

#include <bits/stdc++.h>

namespace lfu_cache_lld {

class CacheStatistics {
public:
    void recordHit() noexcept { hits_.fetch_add(1, std::memory_order_relaxed); }

    void recordMiss() noexcept { misses_.fetch_add(1, std::memory_order_relaxed); }

    void recordPut() noexcept { puts_.fetch_add(1, std::memory_order_relaxed); }

    void recordEviction() noexcept { evictions_.fetch_add(1, std::memory_order_relaxed); }

    void recordRemove() noexcept { removes_.fetch_add(1, std::memory_order_relaxed); }

    [[nodiscard]] long long getHits() const noexcept {
        return hits_.load(std::memory_order_relaxed);
    }

    [[nodiscard]] long long getMisses() const noexcept {
        return misses_.load(std::memory_order_relaxed);
    }

    [[nodiscard]] long long getPuts() const noexcept {
        return puts_.load(std::memory_order_relaxed);
    }

    [[nodiscard]] long long getEvictions() const noexcept {
        return evictions_.load(std::memory_order_relaxed);
    }

    [[nodiscard]] long long getRemoves() const noexcept {
        return removes_.load(std::memory_order_relaxed);
    }

    [[nodiscard]] double getHitRatio() const noexcept {
        const long long total = getHits() + getMisses();
        if (total == 0) {
            return 0.0;
        }
        return static_cast<double>(getHits()) / static_cast<double>(total);
    }

    void reset() noexcept {
        hits_.store(0);
        misses_.store(0);
        puts_.store(0);
        evictions_.store(0);
        removes_.store(0);
    }

    void print() const {
        std::cout << "\n=== Cache Statistics ===\n";
        std::cout << "Policy     : LFU (C++17)\n";
        std::cout << "Hits       : " << getHits() << "\n";
        std::cout << "Misses     : " << getMisses() << "\n";
        std::cout << "Puts       : " << getPuts() << "\n";
        std::cout << "Evictions  : " << getEvictions() << "\n";
        std::cout << "Removes    : " << getRemoves() << "\n";
        std::cout << "Hit Ratio  : " << getHitRatio() * 100.0 << "%\n";
        std::cout << "========================\n";
    }

private:
    std::atomic<long long> hits_{0};
    std::atomic<long long> misses_{0};
    std::atomic<long long> puts_{0};
    std::atomic<long long> evictions_{0};
    std::atomic<long long> removes_{0};
};

} // namespace lfu_cache_lld

#endif // LFU_CACHE_LLD_STATS_CACHESTATISTICS_H

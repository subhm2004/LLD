// ============================================================================
//  stats/CacheStatistics.h  —  Cache ki performance ke counters (thread-safe)
// ----------------------------------------------------------------------------
//  5 numbers ginti hai: hits, misses, puts, evictions, removes. Sabse ahem:
//  HIT RATIO = hits / (hits + misses) — cache kitni "kaam ki" hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `std::atomic` counters — mutex ke bina thread-safe counting           │
//  │                                                                          │
//  │  Multiple threads ek saath recordHit() bula sakte hain. Plain `long long` │
//  │  hota to `++` do steps (read+write) me hota aur threads takra ke count    │
//  │  galat karte (lost update). `atomic::fetch_add` ek indivisible step hai — │
//  │  koi lock nahi, count kabhi galat nahi.                                   │
//  │                                                                          │
//  │  `memory_order_relaxed`: ye SIRF counters hain (kisi ordering/lock ke     │
//  │  liye nahi), to sabse sasta atomic mode — atomicity deta hai bina overhead│
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Ye class cache algorithm se anjaan hai — bas ginti karti hai. Isliye LRU
//     aur LFU dono folder ise (lagbhag same) reuse karte hain (SRP).
// ============================================================================
#ifndef LRU_CACHE_LLD_STATS_CACHESTATISTICS_H
#define LRU_CACHE_LLD_STATS_CACHESTATISTICS_H

#include <atomic>
#include <iostream>
#include <mutex>

namespace lru_cache_lld {

class CacheStatistics {
public:
    // ---- Record (atomic increment — thread-safe) ----------------------------
    void recordHit() { hits_.fetch_add(1, std::memory_order_relaxed); }
    void recordMiss() { misses_.fetch_add(1, std::memory_order_relaxed); }
    void recordPut() { puts_.fetch_add(1, std::memory_order_relaxed); }
    void recordEviction() { evictions_.fetch_add(1, std::memory_order_relaxed); }
    void recordRemove() { removes_.fetch_add(1, std::memory_order_relaxed); }

    // ---- Read (atomic load) -------------------------------------------------
    long long getHits() const { return hits_.load(std::memory_order_relaxed); }
    long long getMisses() const { return misses_.load(std::memory_order_relaxed); }
    long long getPuts() const { return puts_.load(std::memory_order_relaxed); }
    long long getEvictions() const { return evictions_.load(std::memory_order_relaxed); }
    long long getRemoves() const { return removes_.load(std::memory_order_relaxed); }

    // ---- Hit ratio (koi lookup nahi -> 0, div-by-zero guard) ----------------
    double getHitRatio() const {
        long long totalLookups = getHits() + getMisses();
        if (totalLookups == 0) {
            return 0.0;
        }
        return static_cast<double>(getHits()) / static_cast<double>(totalLookups);
    }

    void reset() {
        hits_.store(0);
        misses_.store(0);
        puts_.store(0);
        evictions_.store(0);
        removes_.store(0);
    }

    void print() const {
        std::cout << "\n=== Cache Statistics ===\n";
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

} // namespace lru_cache_lld

#endif // LRU_CACHE_LLD_STATS_CACHESTATISTICS_H

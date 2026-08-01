// ============================================================================
//  stats/CacheStatistics.h  —  Cache ki performance ke counters (thread-safe)
// ----------------------------------------------------------------------------
//  Ye class 5 numbers ginti hai: hits, misses, puts, evictions, removes. In se
//  sabse ahem: HIT RATIO = hits / (hits + misses) — cache kitni "kaam ki" hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `std::atomic` counters — mutex ke bina thread-safe counting           │
//  │                                                                          │
//  │  Multiple threads ek saath recordHit()/recordMiss() bula sakte hain.      │
//  │  Agar plain `long long` hota, to `++` do steps me hota (read+write) aur   │
//  │  do threads beech me takra ke count galat kar dete (lost update).         │
//  │  `std::atomic` `fetch_add` ko EK indivisible step banata hai — koi lock   │
//  │  nahi chahiye, aur count kabhi galat nahi hota.                          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `memory_order_relaxed` — kyun (aur kab safe hai)                      │
//  │                                                                          │
//  │  Ye counters sirf GINTI ke liye hain — inke aas-paas kisi aur data ki     │
//  │  ordering guarantee ki zaroorat nahi (ye kisi lock ki tarah kaam nahi     │
//  │  kar rahe). Relaxed sabse sasta atomic mode hai: atomicity deta hai, par  │
//  │  extra memory-ordering ka overhead nahi. Sirf-counters ke liye perfect.  │
//  │  ⚠ Agar ye counter kisi synchronization ke liye hota (jaise flag), tab    │
//  │     relaxed galat hota — tab acquire/release chahiye hota.               │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 CacheStatistics khud lock nahi leta aur cache algorithm se anjaan hai —
//     wo bas gina ta hai. Isse ise kisi bhi cache (LFU/LRU) ke saath reuse kar
//     sakte hain (Single Responsibility).
// ============================================================================
#ifndef LFU_CACHE_LLD_STATS_CACHESTATISTICS_H
#define LFU_CACHE_LLD_STATS_CACHESTATISTICS_H

#include <atomic>
#include <iostream>

namespace lfu_cache_lld {

class CacheStatistics {
public:
    // ---- Record karo (atomic increment — thread-safe) -----------------------
    void recordHit() noexcept { hits_.fetch_add(1, std::memory_order_relaxed); }
    void recordMiss() noexcept { misses_.fetch_add(1, std::memory_order_relaxed); }
    void recordPut() noexcept { puts_.fetch_add(1, std::memory_order_relaxed); }
    void recordEviction() noexcept { evictions_.fetch_add(1, std::memory_order_relaxed); }
    void recordRemove() noexcept { removes_.fetch_add(1, std::memory_order_relaxed); }

    // ---- Padho (atomic load) ------------------------------------------------
    [[nodiscard]] long long getHits() const noexcept { return hits_.load(std::memory_order_relaxed); }
    [[nodiscard]] long long getMisses() const noexcept { return misses_.load(std::memory_order_relaxed); }
    [[nodiscard]] long long getPuts() const noexcept { return puts_.load(std::memory_order_relaxed); }
    [[nodiscard]] long long getEvictions() const noexcept { return evictions_.load(std::memory_order_relaxed); }
    [[nodiscard]] long long getRemoves() const noexcept { return removes_.load(std::memory_order_relaxed); }

    // ---- Hit ratio = hits / (hits + misses). Koi lookup nahi to 0 (div-by-0 guard) ----
    [[nodiscard]] double getHitRatio() const noexcept {
        const long long total = getHits() + getMisses();
        if (total == 0) {
            return 0.0; // abhi tak koi get nahi hua
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

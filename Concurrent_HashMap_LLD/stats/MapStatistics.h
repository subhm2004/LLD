// ============================================================================
//  stats/MapStatistics.h  —  Map ki performance ke counters (thread-safe)
// ----------------------------------------------------------------------------
//  5 numbers ginti hai: gets, puts, removes, hits, misses. In se sabse ahem:
//  HIT RATE = hits / gets — map "kaam ki" hai ya zyadatar miss ho rahe.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `std::atomic` counters — YAHAN ye SACH me zaroori hain (optional nahi)│
//  │                                                                          │
//  │  StripedHashMap me alag-alag stripes ke alag-alag locks hote hain. Do     │
//  │  threads jo ALAG stripes me kaam kar rahe hain, wo ek saath (bina kisi    │
//  │  COMMON lock ke) `stats_.recordPut()` bula sakte hain. Agar counter plain │
//  │  `long long` hota, `++` do steps me hota aur dono thread takra ke count   │
//  │  galat kar dete (lost update). `atomic::fetch_add` isse ek indivisible    │
//  │  step banata — bina kisi lock ke sahi ginti.                             │
//  │                                                                          │
//  │  💡 Yaani: map ka DATA to per-stripe lock se safe hai, par SHARED stats   │
//  │  object cross-stripe se touch hota — isliye stats ko khud atomic hona     │
//  │  padta. Ye ek fine (par important) concurrency detail hai.               │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `atomic` NON-COPYABLE hota hai — isi wajah se MapStatistics (aur usko rakhne
//     wale maps) apne aap non-copyable ban jaate hain. Ye accha hai: map ki galti
//     se copy hone se bacha lete hain (mutex bhi non-copyable, dono milke guard).
//
//  📌 Ye struct cache/LRU/LFU folder ke CacheStatistics jaisa hi hai — ek reusable
//     "metrics" concern jo core algorithm se alag hai (Single Responsibility).
// ============================================================================
#ifndef CONCURRENT_HASHMAP_LLD_STATS_MAPSTATISTICS_H
#define CONCURRENT_HASHMAP_LLD_STATS_MAPSTATISTICS_H

#include <atomic>
#include <iostream>

namespace concurrent_hashmap_lld {

struct MapStatistics {
    std::atomic<long long> gets{0};    // kitne get calls
    std::atomic<long long> puts{0};    // kitne put calls
    std::atomic<long long> removes{0}; // kitne successful removes
    std::atomic<long long> hits{0};    // get me value mila
    std::atomic<long long> misses{0};  // get me value nahi mila

    // Get record karo + hit/miss classify karo (atomic -> thread-safe).
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

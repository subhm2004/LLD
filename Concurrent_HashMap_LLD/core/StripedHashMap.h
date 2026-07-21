// ============================================================================
//  core/StripedHashMap.h  —  Approach 2: LOCK STRIPING (fine-grained locking)
// ----------------------------------------------------------------------------
//  Ek bade lock ki jagah, N chhote locks. Map ko N "stripes" (tukdon) me baant
//  do — har stripe ka apna mutex AUR apna chhota unordered_map. Key kaunse stripe
//  me jaayegi, wo `hash(key) % N` decide karta hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ ASLI IDEA: ALAG stripes ki keys PARALLEL chal sakti hain              │
//  │                                                                          │
//  │      key "apple" -> hash % 16 = 3  -> stripe 3 (lock 3, bucket 3)         │
//  │      key "mango" -> hash % 16 = 9  -> stripe 9 (lock 9, bucket 9)         │
//  │                                                                          │
//  │  Thread-A "apple" pe kaam kare (lock 3), Thread-B "mango" pe (lock 9) —   │
//  │  dono ALAG locks, to DONO EK SAATH chal sakte hain! CoarseGrained me ye   │
//  │  dono ek doosre ka wait karte (ek hi lock). Yahi striping ka fayda:       │
//  │  parallelism badhta hai (jab keys alag stripes me girti hain).           │
//  │                                                                          │
//  │  ⚠ SAME stripe (ya same key) pe do threads? -> wo phir bhi serialize      │
//  │     honge (ek hi stripe-lock). Striping "poora" parallel nahi deta, bas   │
//  │     contention ko N stripes me BAANT deta hai (average kam wait).         │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `vector<unique_ptr<mutex>>` kyun (seedha vector<mutex> nahi)?         │
//  │                                                                          │
//  │  `std::mutex` NON-COPYABLE aur NON-MOVABLE hai. `vector<mutex>` ko resize │
//  │  karna pade to wo elements ko move/copy karega -> compile error / UB.     │
//  │  Isliye locks ko `unique_ptr<mutex>` me rakhte hain: vector pointers ko   │
//  │  move karta hai (mutex object khud apni jagah stable rehta). Ye classic   │
//  │  "mutex ko container me kaise rakhein" wala C++ solution hai.            │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠ `size()` ka dhyaan: ye SAARE stripes ko ek-ek karke lock karke jodta hai.
//     Ye ek "consistent global snapshot" NAHI hai — jab hum stripe 5 gin rahe
//     hote hain, stripe 0 me koi aur put kar sakta hai. Concurrent map me exact
//     instantaneous size waise bhi meaningless hai; ye best-effort count hai.
//
//  📌 Stripe count (N) ka trade-off: zyada stripes -> zyada parallelism, par
//     zyada memory (N mutex + N map). Aam tor pe N = core count ke aas-paas
//     (ya thoda zyada) rakhte hain. Default 16.
// ============================================================================
#ifndef CONCURRENT_HASHMAP_LLD_CORE_STRIPEDHASHMAP_H
#define CONCURRENT_HASHMAP_LLD_CORE_STRIPEDHASHMAP_H

#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "IConcurrentMap.h"
#include "../stats/MapStatistics.h"

namespace concurrent_hashmap_lld {

class StripedHashMap : public IConcurrentMap {
public:
    // N stripes banao (0 diya to safe default 16). Har stripe ka apna bucket + mutex.
    explicit StripedHashMap(size_t num_stripes = 16)
        : stripes_(num_stripes > 0 ? num_stripes : 16) {
        buckets_.resize(stripes_); // N chhote maps
        locks_.reserve(stripes_);
        for (size_t i = 0; i < stripes_; ++i) {
            locks_.emplace_back(std::make_unique<std::mutex>()); // N alag mutex (upar note)
        }
    }

    void put(const std::string& key, const std::string& value) override {
        size_t idx = stripeIndex(key);                  // ye key kaunse stripe me?
        std::lock_guard<std::mutex> lock(*locks_[idx]); // ⭐ sirf USI stripe ka lock
        buckets_[idx][key] = value;                     // us stripe ke bucket me daalo
        stats_.recordPut();
    }

    std::optional<std::string> get(const std::string& key) const override {
        size_t idx = stripeIndex(key);
        std::lock_guard<std::mutex> lock(*locks_[idx]); // sirf us stripe ka lock
        const auto& bucket = buckets_[idx];
        auto it = bucket.find(key);
        if (it == bucket.end()) {
            stats_.recordGet(false); // miss
            return std::nullopt;
        }
        stats_.recordGet(true);      // hit
        return it->second;
    }

    bool remove(const std::string& key) override {
        size_t idx = stripeIndex(key);
        std::lock_guard<std::mutex> lock(*locks_[idx]);
        bool erased = buckets_[idx].erase(key) > 0;
        if (erased) {
            stats_.recordRemove();
        }
        return erased;
    }

    bool contains(const std::string& key) const override {
        size_t idx = stripeIndex(key);
        std::lock_guard<std::mutex> lock(*locks_[idx]);
        return buckets_[idx].find(key) != buckets_[idx].end();
    }

    // ⚠ Best-effort size: har stripe ko lock karke count jodo (global snapshot nahi).
    size_t size() const override {
        size_t total = 0;
        for (size_t i = 0; i < stripes_; ++i) {
            std::lock_guard<std::mutex> lock(*locks_[i]); // ek waqt me ek stripe lock
            total += buckets_[i].size();
        }
        return total;
    }

    const char* strategyName() const override { return "LockStriping (fine-grained)"; }

    size_t numStripes() const { return stripes_; }

    MapStatistics& statistics() { return stats_; }
    const MapStatistics& statistics() const { return stats_; }

private:
    // Key -> stripe index. std::hash se hash lo, N se modulo -> 0..N-1.
    // ⭐ Ek key HAMESHA usi stripe me jaati hai (deterministic) — isliye put/get/
    //    remove sab sahi stripe pe lockte hain (warna data alag jagah bikhar jaata).
    size_t stripeIndex(const std::string& key) const {
        std::hash<std::string> hasher;
        return hasher(key) % stripes_;
    }

    size_t stripes_;                                                       // kitne stripes (N)
    mutable std::vector<std::unordered_map<std::string, std::string>> buckets_; // N chhote maps
    mutable std::vector<std::unique_ptr<std::mutex>> locks_;              // N alag locks (upar note)
    mutable MapStatistics stats_;                                        // shared metrics (atomic!)
};

}  // namespace concurrent_hashmap_lld

#endif  // CONCURRENT_HASHMAP_LLD_CORE_STRIPEDHASHMAP_H

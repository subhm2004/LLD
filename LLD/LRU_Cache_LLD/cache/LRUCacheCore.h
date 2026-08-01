// ============================================================================
//  cache/LRUCacheCore.h  —  O(1) LRU cache ka ASLI dimaag (single-threaded)
// ----------------------------------------------------------------------------
//  LRU = "Least Recently Used". Jab jagah bhar jaaye, wo item nikaalo jo SABSE
//  DER SE use hua (sabse purana). Har access item ko "taaza" (most-recent) bana
//  deta hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ O(1) KAISE? — LIST + MAP ka combo                                     │
//  │                                                                          │
//  │  1. order_  : std::list<CacheEntry>  (recency order)                     │
//  │       FRONT = most-recently-used (MRU)                                    │
//  │       BACK  = least-recently-used (LRU)  <- evict yahan se               │
//  │                                                                          │
//  │  2. lookup_ : key -> us list me entry ka ITERATOR  (O(1) lookup)          │
//  │                                                                          │
//  │      order_ :  MRU [ D ][ C ][ B ][ A ] LRU                              │
//  │                             ▲                                             │
//  │      lookup_["B"] ──────────┘  (seedha iterator, list scan nahi)          │
//  │                                                                          │
//  │  Access hua -> entry ko FRONT pe le jao (splice). Evict -> BACK hatao.    │
//  │  Dono O(1) kyunki lookup_ se exact iterator milta hai.                   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `std::list::splice` — LRU ka JAADUI operation                        │
//  │                                                                          │
//  │  touch() me: `order_.splice(order_.begin(), order_, iterator)`            │
//  │  Ye entry ko uski jagah se UTHA ke list ke front pe LAGA deta hai —       │
//  │  BINA node ko copy/move kiye, sirf internal pointers re-link karke. O(1). │
//  │  Aur — SPLICE ITERATORS KO INVALIDATE NAHI KARTA! Isliye lookup_ me       │
//  │  stored iterator splice ke baad bhi valid rehta hai. Yahi wajah hai ki    │
//  │  list (vector nahi) use hui — vector me move sab iterators tod deta.     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 LRU vs LFU: LRU sirf "kab" (time) dekhta hai, LFU "kitni baar" (count).
//     Detailed comparison ke liye WHY_LRU_OVER_LFU.md padho. Ye class LFU se
//     SIMPLER hai — koi frequency/minFreq nahi, bas list order.
//
//  📌 Ye class SINGLE-THREADED hai (koi lock nahi). Thread-safety alag layer
//     (ThreadSafeLRUCache) deta hai — SRP: algorithm alag, locking alag.
// ============================================================================
#ifndef LRU_CACHE_LLD_CACHE_LRUCACHECORE_H
#define LRU_CACHE_LLD_CACHE_LRUCACHECORE_H

#include <list>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <utility>

#include "../cache/ICache.h"
#include "../config/CacheConfig.h"
#include "../models/CacheEntry.h"

namespace lru_cache_lld {

template <typename Key, typename Value>
class LRUCacheCore : public ICache<Key, Value> {
public:
    explicit LRUCacheCore(const CacheConfig &config) : config_(config) {}

    // ---- GET: mila to front pe promote karo + value do; nahi mila -> nullopt -
    std::optional<Value> get(const Key &key) override {
        auto iterator = lookup_.find(key);
        if (iterator == lookup_.end()) {
            return std::nullopt; // MISS
        }
        touch(iterator->second);         // ye access -> entry ko MRU (front) banao
        return iterator->second->value;  // iterator->second = list entry ka iterator
    }

    // ---- PUT: naya insert ya purana update (dono me MRU banta hai) ----------
    void put(const Key &key, const Value &value) override {
        lastPutEvicted_ = false;

        auto iterator = lookup_.find(key);
        if (iterator != lookup_.end()) {
            // Pehle se hai -> value update + front pe promote (recent access)
            iterator->second->value = value;
            touch(iterator->second);
            return;
        }

        // Nayi key -> list ke front (MRU) pe daalo, aur lookup me iterator save
        order_.push_front(CacheEntry<Key, Value>(key, value));
        lookup_[key] = order_.begin();

        // ⭐ Capacity cross hui? -> BACK (LRU, sabse purana) ko evict karo.
        // Notice: pehle daala PHIR check — to size temporarily capacity+1 hoti
        // hai, phir turant ek evict. Net: capacity maintain rehti hai.
        if (order_.size() > config_.getCapacity()) {
            evictLeastRecentlyUsed();
        }
    }

    // ---- CONTAINS: sirf check (recency ko HAATH nahi lagata) ----------------
    bool contains(const Key &key) const override {
        return lookup_.find(key) != lookup_.end();
    }

    // ---- REMOVE: list se aur lookup se dono jagah se hatao ------------------
    bool remove(const Key &key) override {
        auto iterator = lookup_.find(key);
        if (iterator == lookup_.end()) {
            return false;
        }
        order_.erase(iterator->second); // list se entry hatao (iterator se O(1))
        lookup_.erase(iterator);        // lookup se key hatao
        return true;
    }

    void clear() override {
        order_.clear();
        lookup_.clear();
    }

    size_t size() const override { return lookup_.size(); }
    size_t capacity() const override { return config_.getCapacity(); }

    // Abhi ke put me eviction hui? (CacheService stats ke liye)
    bool didLastPutEvict() const { return lastPutEvicted_; }
    void resetEvictionFlag() { lastPutEvicted_ = false; }

    // Recency order ka snapshot (debug/test ke liye — front=MRU, back=LRU).
    std::list<CacheEntry<Key, Value>> getRecencyOrderSnapshot() const {
        return order_;
    }

private:
    using EntryList = std::list<CacheEntry<Key, Value>>;
    using EntryIterator = typename EntryList::iterator;

    // ⭐ touch = entry ko front (MRU) pe le jao. splice = O(1), iterators valid rehte.
    void touch(EntryIterator iterator) {
        order_.splice(order_.begin(), order_, iterator);
    }

    // ⭐ Evict = list ke BACK (sabse purana = LRU) ko hatao.
    void evictLeastRecentlyUsed() {
        if (order_.empty()) {
            return; // kuch hai hi nahi (defensive)
        }
        const Key &evictedKey = order_.back().key; // back = LRU
        lookup_.erase(evictedKey);
        order_.pop_back();
        lastPutEvicted_ = true;
    }

    CacheConfig config_;
    EntryList order_;                              // recency list (front=MRU, back=LRU)
    std::unordered_map<Key, EntryIterator> lookup_; // key -> list iterator (O(1))
    bool lastPutEvicted_ = false;
};

} // namespace lru_cache_lld

#endif // LRU_CACHE_LLD_CACHE_LRUCACHECORE_H

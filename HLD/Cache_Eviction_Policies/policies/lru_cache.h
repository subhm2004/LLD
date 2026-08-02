// ============================================================================
//  policies/lru_cache.h  —  LRU (Least Recently Used)
// ----------------------------------------------------------------------------
//  "Jise sabse DER SE use nahi kiya, usko nikalo."
//
//  Soch: agar koi cheez abhi use hui hai, to shayad phir use hogi (temporal
//  locality). Aur jise ghanton se koi nahi pooch raha, wo shayad ab bekaar hai.
//
//  ⭐ Ye soch asli traffic pe aksar SAHI baithti hai — isi liye LRU sabse zyada
//     use hone wali policy hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ O(1) KAISE — list + hashmap ka classic jodaa                          │
//  │                                                                          │
//  │    list<string>                    : order rakhta hai (front = sabse naya)│
//  │    unordered_map<string, iterator> : "ye key list me KAHAN hai"           │
//  │                                                                          │
//  │  Hit pe: map se iterator mila -> `splice` se use front pe le aao. O(1).   │
//  │  `splice` node ko UTHA kar dusri jagah jod deta hai — koi copy nahi,      │
//  │  koi allocation nahi, aur purane iterators bhi valid rehte hain.          │
//  │  (Isi liye `list` chuna, `vector` nahi — vector me shift karna O(n) hota.)│
//  │                                                                          │
//  │  Evict pe: list ke back se uthao (wahi sabse purana). O(1).               │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ❌ ISKI SABSE BADI KAMZORI — SEQUENTIAL SCAN
//     Ek badi table/file poori scan karo, jisme har key sirf EK BAAR aati hai.
//     LRU har nayi key ko andar daalta hai aur usi hisaab se purani (kaam ki!)
//     keys ko bahar phenk deta hai. Scan khatam hone tak poora cache un keys se
//     bhar chuka hota hai jinki dobara zaroorat hi nahi.
//     Ise "cache pollution" ya "scan problem" kehte hain — 01 wala demo ise
//     naap ke dikhata hai.
// ============================================================================
#ifndef CACHE_EVICTION_POLICIES_LRU_CACHE_H
#define CACHE_EVICTION_POLICIES_LRU_CACHE_H

#include <list>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "cache_policy.h"

using namespace std;

namespace cache
{

class LruCache : public CachePolicy
{
public:
    explicit LruCache(size_t capacity) : capacity_(capacity)
    {
        if (capacity == 0)
        {
            throw invalid_argument("capacity 0 se badi honi chahiye");
        }
    }

    const char *name() const override { return "LRU"; }

    bool access(const string &key) override
    {
        auto it = index_.find(key);
        if (it != index_.end())
        {
            // ⭐ HIT — is key ko front pe le aao (ab ye "sabse naya" hai)
            order_.splice(order_.begin(), order_, it->second);
            return true;
        }

        // MISS — jagah nahi hai to sabse PURANA (back) nikalo
        if (order_.size() >= capacity_)
        {
            index_.erase(order_.back());
            order_.pop_back();
        }
        order_.push_front(key);
        index_[key] = order_.begin();
        return false;
    }

    void clear() override
    {
        order_.clear();
        index_.clear();
    }

    size_t capacity() const override { return capacity_; }

private:
    size_t capacity_;
    list<string> order_;                                  // front = sabse naya
    unordered_map<string, list<string>::iterator> index_; // key -> list me jagah
};

} // namespace cache

#endif // CACHE_EVICTION_POLICIES_LRU_CACHE_H

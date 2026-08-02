// ============================================================================
//  policies/random_cache.h  —  RANDOM eviction
// ----------------------------------------------------------------------------
//  "Jagah khatam? Koi bhi utha ke phenk do."
//
//  Ye mazaak nahi hai — ye ek serious baseline hai, aur do wajah se zaroori:
//
//  ⭐ 1. BASELINE — koi bhi policy tabhi useful hai jab wo Random se BEHTAR ho.
//        Agar aapki 200-line "smart" policy random se sirf 1% behtar hai, to
//        wo complexity bekaar hai. Random ise naap ne ka paimana deta hai.
//
//  ⭐ 2. Ye kabhi-kabhi HAIRAAN karta hai. Random ke paas koi order nahi hai,
//        isliye use "scan problem" bhi nahi hoti (05 wale demo me dikhega —
//        sequential scan pe Random LRU se behtar nikal sakta hai!).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ O(1) RANDOM EVICTION — vector + index map ka trick                    │
//  │                                                                          │
//  │  Problem: `unordered_set` me se random element uthana O(n) hai.           │
//  │                                                                          │
//  │  Hal: keys ko `vector` me rakho, aur ek map rakho "ye key vector me kis   │
//  │  index pe hai". Ab random index uthao, aur usko hatane ke liye:           │
//  │      - aakhri element ko us index pe copy karo                            │
//  │      - vector se aakhri element pop karo                                  │
//  │  Dono O(1). Order bigadta hai — par Random ko order ki parwah hi nahi. 😄 │
//  │                                                                          │
//  │  (Yahi "swap-with-last" trick har jagah kaam aata hai jab order matter    │
//  │   na karta ho.)                                                          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Redis me `allkeys-random` sach me ek option hai. Aur Redis ka default
//     `allkeys-lru` bhi asal me APPROXIMATED LRU hai — wo 5 random keys uthata
//     hai aur unme se sabse purani nikaal deta hai. Yaani thoda randomness
//     production me jaan-bujh ke daala jaata hai (exact LRU ki bookkeeping
//     mehngi padti hai).
// ============================================================================
#ifndef CACHE_EVICTION_POLICIES_RANDOM_CACHE_H
#define CACHE_EVICTION_POLICIES_RANDOM_CACHE_H

#include <random>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "cache_policy.h"

using namespace std;

namespace cache
{

class RandomCache : public CachePolicy
{
public:
    explicit RandomCache(size_t capacity, unsigned seed = 42u)
        : capacity_(capacity), rng_(seed)
    {
        if (capacity == 0)
        {
            throw invalid_argument("capacity 0 se badi honi chahiye");
        }
    }

    const char *name() const override { return "Random"; }

    bool access(const string &key) override
    {
        if (index_.count(key))
        {
            return true; // HIT — kuch karne ki zaroorat nahi
        }

        if (keys_.size() >= capacity_)
        {
            evictRandom();
        }
        index_[key] = keys_.size();
        keys_.push_back(key);
        return false;
    }

    void clear() override
    {
        keys_.clear();
        index_.clear();
    }

    size_t capacity() const override { return capacity_; }

private:
    // ⭐ swap-with-last trick — O(1) me kisi bhi element ko hatao
    void evictRandom()
    {
        uniform_int_distribution<size_t> pick(0, keys_.size() - 1);
        size_t victimIndex = pick(rng_);

        const string victim = keys_[victimIndex];
        const string &last = keys_.back();

        keys_[victimIndex] = last;  // aakhri ko victim ki jagah bitha do
        index_[last] = victimIndex; // uska naya index note karo
        keys_.pop_back();
        index_.erase(victim);
    }

    size_t capacity_;
    mt19937 rng_;
    vector<string> keys_;                 // jo cache me hain
    unordered_map<string, size_t> index_; // key -> vector me uska index
};

} // namespace cache

#endif // CACHE_EVICTION_POLICIES_RANDOM_CACHE_H

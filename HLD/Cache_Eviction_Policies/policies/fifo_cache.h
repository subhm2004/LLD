// ============================================================================
//  policies/fifo_cache.h  —  FIFO (First In, First Out)
// ----------------------------------------------------------------------------
//  "Jo sabse pehle AAYA tha, wahi sabse pehle jaayega."
//
//  Sabse simple policy — bas ek queue. Koi bookkeeping nahi, hit pe kuch karna
//  hi nahi hota.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠ FIFO aur LRU ka farak — ye interview me poocha jaata hai              │
//  │                                                                          │
//  │  Dono queue jaise lagte hain, par ek bahut bada farak hai:                │
//  │                                                                          │
//  │     LRU  -> HIT pe key ko wapas front pe le aata hai                      │
//  │     FIFO -> HIT pe KUCH NAHI karta, order wahi rehta hai                  │
//  │                                                                          │
//  │  Yaani FIFO ke liye "kitni popular hai" ka koi matlab hi nahi. Ek key     │
//  │  1000 baar hit ho rahi ho, phir bhi apni baari aane pe nikal jaayegi.     │
//  │                                                                          │
//  │  Isi liye FIFO aksar LRU se kharaab karta hai — par uska code sabse       │
//  │  chhota hai aur hit path pe ZERO kaam hai.                               │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Asli duniya me FIFO akela kam use hota hai, par uska sudhra hua roop
//     (CLOCK / second-chance) OS aur DB me har jagah hai — kyunki wo FIFO ki
//     sastai aur LRU ki samajh, dono thoda-thoda de deta hai.
// ============================================================================
#ifndef CACHE_EVICTION_POLICIES_FIFO_CACHE_H
#define CACHE_EVICTION_POLICIES_FIFO_CACHE_H

#include <list>
#include <stdexcept>
#include <string>
#include <unordered_set>

#include "cache_policy.h"

using namespace std;

namespace cache
{

class FifoCache : public CachePolicy
{
public:
    explicit FifoCache(size_t capacity) : capacity_(capacity)
    {
        if (capacity == 0)
        {
            throw invalid_argument("capacity 0 se badi honi chahiye");
        }
    }

    const char *name() const override { return "FIFO"; }

    bool access(const string &key) override
    {
        if (present_.count(key))
        {
            return true; // ⭐ HIT — aur order me KUCH nahi badalta (yahi LRU se farak)
        }

        if (order_.size() >= capacity_)
        {
            present_.erase(order_.front());
            order_.pop_front(); // sabse pehle aaya tha, sabse pehle gaya
        }
        order_.push_back(key);
        present_.insert(key);
        return false;
    }

    void clear() override
    {
        order_.clear();
        present_.clear();
    }

    size_t capacity() const override { return capacity_; }

private:
    size_t capacity_;
    list<string> order_;            // front = sabse pehle aaya
    unordered_set<string> present_; // O(1) me "hai kya?"
};

} // namespace cache

#endif // CACHE_EVICTION_POLICIES_FIFO_CACHE_H

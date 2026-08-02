// ============================================================================
//  policies/clock_cache.h  —  CLOCK (Second-Chance) — "gareebon ka LRU"
// ----------------------------------------------------------------------------
//  LRU achha hai, par uski keemat hai: HAR hit pe list me node ko utha kar
//  front pe le jaana padta hai. Ek busy cache me ye lakhon baar hota hai, aur
//  multi-threaded system me us list pe LOCK bhi lagana padta hai — wo lock
//  poore cache ka bottleneck ban jaata hai.
//
//  ⭐ CLOCK ka jugaad: har entry ke saath ek REFERENCE BIT rakho.
//        HIT pe        -> bas bit = 1 kar do (koi list nahi hilti! bas ek bit)
//        EVICT karna ho -> ek ghadi ki sui (hand) ghumao:
//                            bit == 1 mila? -> "doosra mauka" do, bit = 0 karo, aage badho
//                            bit == 0 mila? -> yahi victim hai, ise nikaalo
//
//  Yaani jo entry pichhle chakkar me use hui thi wo ek baar bach jaati hai.
//  Isse LRU jaisa behaviour "lagbhag" mil jaata hai — par hit path pe sirf ek
//  bit likhna padta hai, jo bahut sasta aur lock-friendly hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ NAAM "CLOCK" KYUN                                                     │
//  │                                                                          │
//  │  Saare slots ek CIRCLE me sochiye, aur ek sui (hand_) us circle pe        │
//  │  ghoomti hai. Jab jagah chahiye hoti hai to sui aage badhti hai aur       │
//  │  pehla "bit == 0" wala slot uthaa leti hai. Sui wahin ruk jaati hai —     │
//  │  agli baar wahin se shuru hogi.                                          │
//  │                                                                          │
//  │  ⚠ Worst case me sui poora chakkar laga sakti hai (agar saare bits 1      │
//  │     hon) — par tab wo sabke bits 0 kar chuki hoti hai, to doosre chakkar  │
//  │     me pakka victim mil jaata hai. Isliye ye kabhi atakta nahi.           │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Ye "academic" cheez nahi hai — Linux ka page cache, PostgreSQL ka buffer
//     manager, aur bahut se DB isi family ka algorithm use karte hain. Exact
//     LRU production me aksar bahut mehnga padta hai.
// ============================================================================
#ifndef CACHE_EVICTION_POLICIES_CLOCK_CACHE_H
#define CACHE_EVICTION_POLICIES_CLOCK_CACHE_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "cache_policy.h"

using namespace std;

namespace cache
{

class ClockCache : public CachePolicy
{
public:
    explicit ClockCache(size_t capacity) : capacity_(capacity)
    {
        if (capacity == 0)
        {
            throw invalid_argument("capacity 0 se badi honi chahiye");
        }
        slots_.reserve(capacity);
    }

    const char *name() const override { return "CLOCK"; }

    bool access(const string &key) override
    {
        auto it = index_.find(key);
        if (it != index_.end())
        {
            // ⭐ HIT — poora kaam bas ye ek line hai. Koi list nahi hilti.
            slots_[it->second].referenced = true;
            return true;
        }

        // MISS — abhi jagah bachi hai to seedha daal do
        if (slots_.size() < capacity_)
        {
            index_[key] = slots_.size();
            slots_.push_back({key, true});
            return false;
        }

        // Cache full — sui ghumao aur victim dhoondho
        size_t victimSlot = findVictim();
        index_.erase(slots_[victimSlot].key);
        slots_[victimSlot] = {key, true};
        index_[key] = victimSlot;
        return false;
    }

    void clear() override
    {
        slots_.clear();
        index_.clear();
        hand_ = 0;
    }

    size_t capacity() const override { return capacity_; }

private:
    struct Slot
    {
        string key;
        bool referenced;
    };

    // ⭐ Sui ghumao: bit 1 wale ko doosra mauka (bit 0 karke aage), bit 0 wala victim
    size_t findVictim()
    {
        while (true)
        {
            Slot &slot = slots_[hand_];
            if (!slot.referenced)
            {
                size_t victim = hand_;
                hand_ = (hand_ + 1) % slots_.size(); // agli baar yahan se shuru
                return victim;
            }
            slot.referenced = false; // doosra mauka mila, ab agli baar nahi
            hand_ = (hand_ + 1) % slots_.size();
        }
    }

    size_t capacity_;
    size_t hand_ = 0; // ghadi ki sui
    vector<Slot> slots_;
    unordered_map<string, size_t> index_; // key -> slot number
};

} // namespace cache

#endif // CACHE_EVICTION_POLICIES_CLOCK_CACHE_H

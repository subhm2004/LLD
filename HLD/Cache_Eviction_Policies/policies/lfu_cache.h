// ============================================================================
//  policies/lfu_cache.h  —  LFU (Least Frequently Used)
// ----------------------------------------------------------------------------
//  "Jise sabse KAM baar use kiya, usko nikalo."
//
//  Soch LRU se alag hai: LRU dekhta hai "kab" use hua, LFU dekhta hai "kitni
//  baar". Agar koi cheez 1000 baar maangi gayi hai to wo sach me popular hai —
//  bhale hi pichhle 5 minute se koi na aaya ho.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ O(1) KAISE — "frequency buckets" + minFreq                            │
//  │                                                                          │
//  │  Seedha tareeka hota: saari keys me se sabse kam count wali dhoondho —    │
//  │  par wo O(n) hai. Iski jagah:                                            │
//  │                                                                          │
//  │    buckets_[f] = un saari keys ki list jinka count THEEK f hai            │
//  │    minFreq_    = abhi ka sabse chhota f                                  │
//  │                                                                          │
//  │  Hit pe : key ko bucket[f] se bucket[f+1] me splice karo. O(1).           │
//  │           Agar bucket[f] khaali ho gaya aur f == minFreq -> minFreq++     │
//  │  Evict pe: bucket[minFreq] ke BACK se uthao. O(1).                        │
//  │                                                                          │
//  │  ⭐ Ek bucket ke andar bhi order rakhte hain (LRU jaisa) — taaki same     │
//  │     frequency wali do keys me se purani pehle nikle. Isi liye har bucket  │
//  │     ek `list` hai, sirf ek set nahi.                                     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ❌ ISKI SABSE BADI KAMZORI — "CACHE POLLUTION" / AGING problem
//     Kal ki viral video ka count 50,000 pahunch gaya. Aaj wo bilkul dead hai,
//     par uska count itna bada hai ki naya trending content (count 5, 10, 20)
//     usko hila hi nahi sakta. Purani cheez cache me CHIPAK jaati hai.
//
//     Iska matlab: LFU tab achha hai jab popularity STABLE ho. Jab popularity
//     BADALTI rehti hai (news, social media, trends) to LFU pichhad jaata hai.
//     02 wala demo ise naap ke dikhata hai.
//
//     📌 Asli systems isi liye "LFU with aging" ya TinyLFU use karte hain,
//        jahan purane counts dheere-dheere ghatte rehte hain.
// ============================================================================
#ifndef CACHE_EVICTION_POLICIES_LFU_CACHE_H
#define CACHE_EVICTION_POLICIES_LFU_CACHE_H

#include <list>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "cache_policy.h"

using namespace std;

namespace cache
{

class LfuCache : public CachePolicy
{
public:
    explicit LfuCache(size_t capacity) : capacity_(capacity)
    {
        if (capacity == 0)
        {
            throw invalid_argument("capacity 0 se badi honi chahiye");
        }
    }

    const char *name() const override { return "LFU"; }

    bool access(const string &key) override
    {
        auto it = entries_.find(key);
        if (it != entries_.end())
        {
            touch(it->second); // ⭐ HIT — frequency badhao
            return true;
        }

        // MISS — jagah nahi hai to sabse kam frequency wali nikalo
        if (entries_.size() >= capacity_)
        {
            evict();
        }

        // Nayi key hamesha frequency 1 se shuru hoti hai
        buckets_[1].push_front(key);
        entries_[key] = {1, buckets_[1].begin()};
        minFreq_ = 1; // naya item aaya matlab min ab 1 hai
        return false;
    }

    void clear() override
    {
        entries_.clear();
        buckets_.clear();
        minFreq_ = 0;
    }

    size_t capacity() const override { return capacity_; }

private:
    struct Entry
    {
        int frequency;
        list<string>::iterator position;
    };

    // Frequency f se f+1 ke bucket me le jao
    void touch(Entry &entry)
    {
        int oldFreq = entry.frequency;
        int newFreq = oldFreq + 1;

        list<string> &oldBucket = buckets_[oldFreq];
        list<string> &newBucket = buckets_[newFreq];

        // splice = node utha kar dusri list me jod do (koi copy nahi)
        newBucket.splice(newBucket.begin(), oldBucket, entry.position);
        entry.frequency = newFreq;
        entry.position = newBucket.begin();

        // ⭐ Agar purana bucket khaali ho gaya aur wahi minimum tha, to min badha do
        if (oldBucket.empty())
        {
            buckets_.erase(oldFreq);
            if (minFreq_ == oldFreq)
            {
                minFreq_ = newFreq;
            }
        }
    }

    void evict()
    {
        list<string> &bucket = buckets_[minFreq_];
        // Same frequency me se sabse PURANI (back) — yahi LRU wala tie-break
        const string victim = bucket.back();
        bucket.pop_back();
        if (bucket.empty())
        {
            buckets_.erase(minFreq_);
        }
        entries_.erase(victim);
    }

    size_t capacity_;
    int minFreq_ = 0;
    unordered_map<string, Entry> entries_;
    unordered_map<int, list<string>> buckets_; // frequency -> us frequency ki keys
};

} // namespace cache

#endif // CACHE_EVICTION_POLICIES_LFU_CACHE_H

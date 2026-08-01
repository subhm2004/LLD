// ============================================================================
//  models/CacheEntry.h  —  LRU cache ka ek entry (key + value pair)
// ----------------------------------------------------------------------------
//  Bahut simple: ek key aur uski value. Ye entries `std::list<CacheEntry>`
//  (recency order) me rehti hain. List me entry ki JAGAH hi uski recency batati
//  hai (front = recent, back = purana) — isi liye entry me alag "frequency" ya
//  "timestamp" field ki zaroorat nahi.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ LRU entry LFU node se SIMPLE kyun hai?                                │
//  │                                                                          │
//  │     LFU node -> key + value + FREQUENCY + bucketIterator (4 cheez)        │
//  │     LRU entry -> key + value                (bas 2 cheez!)               │
//  │                                                                          │
//  │  LRU me "recency" ki poori jaankari list ke ORDER me hai — entry ko khud  │
//  │  kuch yaad rakhne ki zaroorat nahi. Iterator lookup_ map me alag stored   │
//  │  hai (entry ke andar nahi). Yahi LRU ko LFU se simpler banata hai.       │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 `key` bhi store karte hain (sirf value nahi) — kyunki eviction ke waqt
//     list ke back se entry milti hai, aur us entry ki KEY chahiye hoti hai
//     lookup_ map se usko erase karne ke liye. Bina key ke pata na chalta kaunsi
//     map-entry hatani hai.
// ============================================================================
#ifndef LRU_CACHE_LLD_MODELS_CACHEENTRY_H
#define LRU_CACHE_LLD_MODELS_CACHEENTRY_H

#include <utility>

namespace lru_cache_lld {

template <typename Key, typename Value>
struct CacheEntry {
    Key key;     // eviction ke waqt lookup_ map se hatane ke liye chahiye
    Value value; // asli data

    // key/value ko move karke set (copy bachti hai).
    CacheEntry(Key keyValue, Value valueData)
        : key(std::move(keyValue)), value(std::move(valueData)) {}
};

} // namespace lru_cache_lld

#endif // LRU_CACHE_LLD_MODELS_CACHEENTRY_H

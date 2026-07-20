// ============================================================================
//  cache/ICache.h  —  Cache ka CONTRACT (Strategy Pattern ka interface)
// ----------------------------------------------------------------------------
//  Ye generic (template) interface batata hai "ek cache kya-kya karega" — get,
//  put, contains, remove, clear, size, capacity. Kaise karega wo har concrete
//  cache (LRU / LFU / FIFO...) apne tarike se decide karti hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ STRATEGY PATTERN — eviction policy ko plug-and-play banao             │
//  │                                                                          │
//  │  Client sirf `ICache<K,V>*` pakadta hai — usse pata nahi andar LRU chal   │
//  │  raha hai ya LFU. Is repo me LRU_Cache_LLD aur LFU_Cache_LLD DONO yahi    │
//  │  same interface implement karte hain, sirf algorithm alag. Policy badalni │
//  │  ho to concrete class swap karo — client code untouched (Open/Closed).    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `std::optional<Value>` return kyun? get() "mila / nahi mila" dono bata
//     sakta hai bina exception ke: mila -> value, nahi -> std::nullopt. "Miss"
//     ek normal return hai, error nahi.
//
//  ⭐ VIRTUAL DESTRUCTOR: client base pointer (`ICache*`) se derived delete kar
//     sakta hai — bina virtual destructor ke derived ka cleanup chalta hi nahi
//     (undefined behavior). Rule: interface = virtual destructor.
// ============================================================================
#ifndef LRU_CACHE_LLD_CACHE_ICACHE_H
#define LRU_CACHE_LLD_CACHE_ICACHE_H

#include <cstddef>
#include <optional>

namespace lru_cache_lld {

template <typename Key, typename Value>
class ICache {
public:
    virtual ~ICache() = default;

    virtual std::optional<Value> get(const Key &key) = 0;         // mila -> value, nahi -> nullopt
    virtual void put(const Key &key, const Value &value) = 0;     // insert/update
    virtual bool contains(const Key &key) const = 0;             // check (recency chhede bina)
    virtual bool remove(const Key &key) = 0;                     // hatao (thi -> true)
    virtual void clear() = 0;                                    // sab khaali
    virtual size_t size() const = 0;                            // abhi kitne
    virtual size_t capacity() const = 0;                       // max kitne
};

} // namespace lru_cache_lld

#endif // LRU_CACHE_LLD_CACHE_ICACHE_H

// ============================================================================
//  cache/ICache.h  —  Cache ka CONTRACT (Strategy Pattern ka interface)
// ----------------------------------------------------------------------------
//  Ye ek generic (template) interface hai jo batata hai "ek cache kya-kya kaam
//  karega" — get, put, contains, remove, clear, size, capacity. Kaise karega,
//  wo har concrete cache (LFU / LRU / FIFO...) apne tarike se decide karti hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ STRATEGY PATTERN — eviction policy ko plug-and-play banao             │
//  │                                                                          │
//  │  Client sirf `ICache<K,V>*` pakadta hai — usse pata nahi andar LFU chal   │
//  │  raha hai ya LRU. Kal ko policy badalni ho (LFU -> LRU), to bas doosri    │
//  │  concrete class inject kar do; client code ka ek line nahi badlega.       │
//  │  Yahi is repo ka do-folder (LRU_Cache_LLD + LFU_Cache_LLD) design hai —   │
//  │  dono SAME interface implement karte hain, sirf algorithm alag.           │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `std::optional<Value>` return kyun (bare Value nahi)?
//     get() "mila ya nahi mila" dono bata sakta hai bina exception ke:
//        mila     -> optional me value
//        nahi mila -> std::nullopt (khaali)
//     Isse "miss" ek normal return hai, error nahi. Caller `.has_value()` se check karta.
//
//  📌 `[[nodiscard]]` — agar caller get()/size() ka result ignore kar de to
//     compiler warning deta hai ("bekaar call kiya?"). Chhoti safety.
//
//  ⭐ VIRTUAL DESTRUCTOR: client base pointer (`ICache*`) se derived object
//     delete kar sakta hai — bina virtual destructor ke derived ka cleanup
//     chalta hi nahi (undefined behavior). Rule: interface = virtual destructor.
// ============================================================================
#ifndef LFU_CACHE_LLD_CACHE_ICACHE_H
#define LFU_CACHE_LLD_CACHE_ICACHE_H

#include <cstddef>
#include <optional>

namespace lfu_cache_lld {

template <typename Key, typename Value>
class ICache {
public:
    virtual ~ICache() = default;

    // Key dhoondho. Mila -> value, nahi mila -> nullopt.
    [[nodiscard]] virtual std::optional<Value> get(const Key &key) = 0;

    // Key daalo/update karo (naya insert ya purana overwrite).
    virtual void put(const Key &key, const Value &value) = 0;

    // Key hai ya nahi (recency/frequency ko chhede bina — read-only check).
    [[nodiscard]] virtual bool contains(const Key &key) const = 0;

    // Key hatao. Hata di -> true, thi hi nahi -> false.
    [[nodiscard]] virtual bool remove(const Key &key) = 0;

    // Sab kuch khaali karo.
    virtual void clear() = 0;

    [[nodiscard]] virtual std::size_t size() const = 0;     // abhi kitne items
    [[nodiscard]] virtual std::size_t capacity() const = 0; // max kitne items
};

} // namespace lfu_cache_lld

#endif // LFU_CACHE_LLD_CACHE_ICACHE_H

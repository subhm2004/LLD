// ============================================================================
//  config/CacheConfig.h  —  Cache ki settings (capacity)
// ----------------------------------------------------------------------------
//  Cache ki capacity (max kitne items) rakhta hai. Alag class isliye taaki aage
//  aur settings (TTL, shard-count) yahin add ho sakein — cache ka constructor
//  har baar na badle.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ VALIDATION CONSTRUCTOR ME (aur setCapacity me bhi) — always-valid     │
//  │                                                                          │
//  │  capacity 0 = bekaar cache. Constructor aur setCapacity DONO 0 pe        │
//  │  exception phenkte hain, to invalid config exist hi nahi kar sakti.      │
//  │  "Fail fast + always-valid object" — baaki code capacity>0 maan ke chalta.│
//  │                                                                          │
//  │  💡 LFU wale CacheConfig me `setCapacity` nahi tha (immutable). Yahan     │
//  │     `setCapacity` hai (mutable) — dynamic resize ke liye. Chhota          │
//  │     design farak, dono valid; requirement ke hisaab se choose karo.      │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef LRU_CACHE_LLD_CONFIG_CACHECONFIG_H
#define LRU_CACHE_LLD_CONFIG_CACHECONFIG_H

#include <stdexcept>

namespace lru_cache_lld {

class CacheConfig {
public:
    explicit CacheConfig(size_t capacity) : capacity_(capacity) {
        if (capacity_ == 0) {
            throw std::invalid_argument("Cache capacity must be greater than 0");
        }
    }

    size_t getCapacity() const { return capacity_; }

    // Dynamic resize — par 0 phir bhi allowed nahi (same guard).
    void setCapacity(size_t capacity) {
        if (capacity == 0) {
            throw std::invalid_argument("Cache capacity must be greater than 0");
        }
        capacity_ = capacity;
    }

private:
    size_t capacity_; // max items (kabhi 0 nahi)
};

} // namespace lru_cache_lld

#endif // LRU_CACHE_LLD_CONFIG_CACHECONFIG_H

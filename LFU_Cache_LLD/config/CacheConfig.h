// ============================================================================
//  config/CacheConfig.h  —  Cache ki settings (abhi sirf capacity)
// ----------------------------------------------------------------------------
//  Ek chhota config object jo cache ki capacity (max kitne items) rakhta hai.
//  Alag class isliye taaki aage aur settings (TTL, shard-count, policy) yahin
//  add ho sakein — cache classes ka constructor har baar na badle.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ VALIDATION CONSTRUCTOR ME — galat config bana hi nahi sakta           │
//  │                                                                          │
//  │  capacity 0 ka matlab "cache jo kuch bhi store na kare" — bekaar/buggy.   │
//  │  Constructor hi 0 pe exception phenk deta hai, to ek INVALID CacheConfig  │
//  │  object exist hi nahi kar sakta. Ye "fail fast + always-valid object"     │
//  │  design hai — baaki code ko capacity>0 maan ke chalne ki azaadi milti.   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 `noexcept` getter pe — ye kabhi throw nahi karta, compiler ko bata dete
//     hain (optimization + clarity). `std::size_t` (unsigned) capacity ke liye
//     sahi type hai (size kabhi negative nahi hota).
// ============================================================================
#ifndef LFU_CACHE_LLD_CONFIG_CACHECONFIG_H
#define LFU_CACHE_LLD_CONFIG_CACHECONFIG_H

#include <cstddef>
#include <stdexcept>

namespace lfu_cache_lld {

class CacheConfig {
public:
    explicit CacheConfig(std::size_t capacity) : capacity_(capacity) {
        if (capacity_ == 0U) {
            throw std::invalid_argument("Cache capacity must be greater than 0");
        }
    }

    std::size_t getCapacity() const noexcept { return capacity_; }

private:
    std::size_t capacity_; // max items (kabhi 0 nahi — constructor guarantee deta hai)
};

} // namespace lfu_cache_lld

#endif // LFU_CACHE_LLD_CONFIG_CACHECONFIG_H

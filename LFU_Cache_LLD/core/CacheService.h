// ============================================================================
//  core/CacheService.h  —  Cache ka FACADE (cache + statistics ek jagah)
// ----------------------------------------------------------------------------
//  Client ke liye "cache" yahi class hai. Andar ThreadSafeLFUCache (asli cache)
//  aur CacheStatistics (hit/miss counters) dono hain. CacheService har operation
//  ko cache pe forward karta hai AUR saath me statistics update kar deta hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ FACADE PATTERN — do cheezon ko ek saaf interface me jodo             │
//  │                                                                          │
//  │  Client ko alag-alag cache aur statistics manage nahi karne padte. Wo     │
//  │  `service.get(key)` bulata hai; andar:                                    │
//  │     1. cache se value nikalti hai                                         │
//  │     2. hit/miss stats khud update ho jaate hain                          │
//  │  Ye "cross-cutting" kaam (metrics) ek jagah wrap ho gaya — cache core     │
//  │  ko iski khabar tak nahi (SRP). Client ek hi patla interface dekhta.     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Statistics is layer pe kyun (cache ke andar nahi)? Taaki cache algorithm
//     (LFU) apne kaam pe focus rahe, aur "metrics kaise count karein" ek alag
//     concern rahe. Kal ko LRU cache aaye to wahi statistics class reuse ho jaati.
// ============================================================================
#ifndef LFU_CACHE_LLD_CORE_CACHESERVICE_H
#define LFU_CACHE_LLD_CORE_CACHESERVICE_H

#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>

#include "../cache/ThreadSafeLFUCache.h"
#include "../config/CacheConfig.h"
#include "../enums/CacheOperationType.h"
#include "../enums/EvictionPolicyType.h"
#include "../stats/CacheStatistics.h"

namespace lfu_cache_lld {

template <typename Key, typename Value>
class CacheService {
public:
    explicit CacheService(const CacheConfig &config) : cache_(config) {}

    // Copy band — CacheService ek cache ko own karta hai, share nahi.
    CacheService(const CacheService &) = delete;
    CacheService &operator=(const CacheService &) = delete;

    // ---- GET: value do + hit/miss stat update -------------------------------
    [[nodiscard]] std::optional<Value> get(const Key &key) {
        const std::optional<Value> value = cache_.get(key);
        if (value.has_value()) {
            statistics_.recordHit();  // mila
        } else {
            statistics_.recordMiss(); // nahi mila
        }
        return value;
    }

    // ---- PUT: daalo + put stat, aur agar eviction hui to eviction stat ------
    void put(const Key &key, const Value &value) {
        cache_.put(key, value);
        statistics_.recordPut();
        // ⭐ Cache batata hai "is put me kisi ko nikaala kya?" — agar haan to count.
        if (cache_.didLastPutEvict()) {
            statistics_.recordEviction();
        }
    }

    [[nodiscard]] bool contains(const Key &key) const { return cache_.contains(key); }

    [[nodiscard]] bool remove(const Key &key) {
        const bool removed = cache_.remove(key);
        if (removed) {
            statistics_.recordRemove();
        }
        return removed;
    }

    void clear() { cache_.clear(); }

    [[nodiscard]] std::size_t size() const { return cache_.size(); }
    [[nodiscard]] std::size_t capacity() const { return cache_.capacity(); }
    [[nodiscard]] int getFrequency(const Key &key) const { return cache_.getFrequency(key); }

    [[nodiscard]] const CacheStatistics &getStatistics() const { return statistics_; }
    void printStatistics() const { statistics_.print(); }

    void printState() const {
        std::cout << "\n=== Cache State (LFU) ===\n";
        std::cout << "Policy   : " << evictionPolicyTypeToString(EvictionPolicyType::LFU) << "\n";
        std::cout << "Size     : " << size() << "\n";
        std::cout << "Capacity : " << capacity() << "\n";
        std::cout << "=========================\n";
    }

    // ========================================================================
    //  DEMO HELPER — ek operation chala ke uska readable description lauta do
    // ========================================================================
    //  Ye enum-driven runner hai: operation type ke hisaab se sahi method bula
    //  ke ek line banata hai ("GET key=A => HIT value=alpha freq=4"). Demo code
    //  ko clean rakhta hai (loop me operations "data" ki tarah paas karo).
    [[nodiscard]] std::string executeAndDescribe(
        CacheOperationType operation, const Key &key,
        const std::optional<Value> &value = std::nullopt) {
        std::ostringstream stream;
        stream << cacheOperationTypeToString(operation) << " key=" << key;

        switch (operation) {
        case CacheOperationType::GET: {
            if (const std::optional<Value> result = get(key)) {
                stream << " => HIT value=" << result.value() << " freq=" << getFrequency(key);
            } else {
                stream << " => MISS";
            }
            break;
        }
        case CacheOperationType::PUT: {
            if (!value.has_value()) {
                throw std::invalid_argument("PUT requires a value"); // put bina value ke nahi
            }
            put(key, value.value());
            stream << " value=" << value.value() << " => OK freq=" << getFrequency(key);
            break;
        }
        case CacheOperationType::CONTAINS:
            stream << " => " << (contains(key) ? "true" : "false");
            if (contains(key)) {
                stream << " freq=" << getFrequency(key);
            }
            break;
        case CacheOperationType::REMOVE:
            stream << " => " << (remove(key) ? "removed" : "not_found");
            break;
        case CacheOperationType::CLEAR:
            clear();
            stream << " => cache cleared";
            break;
        }

        return stream.str();
    }

private:
    ThreadSafeLFUCache<Key, Value> cache_; // asli thread-safe cache
    CacheStatistics statistics_;           // hit/miss/evict counters
};

} // namespace lfu_cache_lld

#endif // LFU_CACHE_LLD_CORE_CACHESERVICE_H

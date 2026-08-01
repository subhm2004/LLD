// ============================================================================
//  core/CacheService.h  —  Cache ka FACADE (cache + statistics ek jagah)
// ----------------------------------------------------------------------------
//  Client ke liye "cache" yahi class hai. Andar ThreadSafeLRUCache (asli cache)
//  aur CacheStatistics (hit/miss counters) dono. CacheService har operation ko
//  cache pe forward karta hai AUR saath me statistics update kar deta hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ FACADE PATTERN — do cheezon ko ek saaf interface me jodo             │
//  │                                                                          │
//  │  Client ko alag-alag cache aur statistics manage nahi karne padte. Wo     │
//  │  `service.get(key)` bulata hai; andar cache se value + hit/miss stats      │
//  │  khud update. Metrics (cross-cutting concern) ek jagah wrap ho gaya —     │
//  │  cache core ko iski khabar tak nahi (SRP).                                │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Statistics is layer pe (cache ke andar nahi) taaki LRU algorithm apne kaam
//     pe focus rahe. Wahi CacheStatistics class LFU folder me bhi reuse hoti hai.
// ============================================================================
#ifndef LRU_CACHE_LLD_CORE_CACHESERVICE_H
#define LRU_CACHE_LLD_CORE_CACHESERVICE_H

#include <iostream>
#include <optional>
#include <sstream>
#include <string>

#include "../cache/ThreadSafeLRUCache.h"
#include "../config/CacheConfig.h"
#include "../enums/CacheOperationType.h"
#include "../stats/CacheStatistics.h"

namespace lru_cache_lld {

template <typename Key, typename Value>
class CacheService {
public:
    explicit CacheService(const CacheConfig &config) : cache_(config) {}

    // ---- GET: value do + hit/miss stat --------------------------------------
    std::optional<Value> get(const Key &key) {
        std::optional<Value> value = cache_.get(key);
        if (value.has_value()) {
            statistics_.recordHit();
        } else {
            statistics_.recordMiss();
        }
        return value;
    }

    // ---- PUT: daalo + put stat, eviction hui to eviction stat ---------------
    void put(const Key &key, const Value &value) {
        cache_.put(key, value);
        statistics_.recordPut();
        if (cache_.didLastPutEvict()) { // cache batata hai eviction hui kya
            statistics_.recordEviction();
        }
    }

    bool contains(const Key &key) const { return cache_.contains(key); }

    bool remove(const Key &key) {
        bool removed = cache_.remove(key);
        if (removed) {
            statistics_.recordRemove();
        }
        return removed;
    }

    void clear() { cache_.clear(); }

    size_t size() const { return cache_.size(); }
    size_t capacity() const { return cache_.capacity(); }

    const CacheStatistics &getStatistics() const { return statistics_; }
    void printStatistics() const { statistics_.print(); }

    void printState() const {
        std::cout << "\n=== Cache State ===\n";
        std::cout << "Size     : " << size() << "\n";
        std::cout << "Capacity : " << capacity() << "\n";
        std::cout << "===================\n";
    }

    // ========================================================================
    //  DEMO HELPER — ek operation chala ke uska readable description lauta do
    // ========================================================================
    //  Enum-driven runner: operation ke hisaab se sahi method bula ke ek line
    //  banata hai ("GET key=user:2 => HIT value=Bob"). Demo code clean rehta.
    std::string executeAndDescribe(CacheOperationType operation, const Key &key,
                                   const std::optional<Value> &value = std::nullopt) {
        std::ostringstream stream;
        stream << cacheOperationTypeToString(operation) << " key=" << key;

        switch (operation) {
        case CacheOperationType::GET: {
            std::optional<Value> result = get(key);
            if (result.has_value()) {
                stream << " => HIT value=" << result.value();
            } else {
                stream << " => MISS";
            }
            break;
        }
        case CacheOperationType::PUT: {
            if (!value.has_value()) {
                throw std::invalid_argument("PUT requires a value");
            }
            put(key, value.value());
            stream << " value=" << value.value() << " => OK";
            break;
        }
        case CacheOperationType::CONTAINS:
            stream << " => " << (contains(key) ? "true" : "false");
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
    ThreadSafeLRUCache<Key, Value> cache_; // asli thread-safe cache
    CacheStatistics statistics_;           // hit/miss/evict counters
};

} // namespace lru_cache_lld

#endif // LRU_CACHE_LLD_CORE_CACHESERVICE_H

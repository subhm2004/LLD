#ifndef LFU_CACHE_LLD_CORE_CACHESERVICE_H
#define LFU_CACHE_LLD_CORE_CACHESERVICE_H

#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>

#include "../cache/ThreadSafeLFUCache.h"
#include "../config/CacheConfig.h"
#include "../config/CppStandard.h"
#include "../enums/CacheOperationType.h"
#include "../enums/EvictionPolicyType.h"
#include "../stats/CacheStatistics.h"

namespace lfu_cache_lld {

template <typename Key, typename Value>
class CacheService {
public:
    explicit CacheService(const CacheConfig &config) : cache_(config) {}

    CacheService(const CacheService &) = delete;
    CacheService &operator=(const CacheService &) = delete;

    [[nodiscard]] std::optional<Value> get(const Key &key) {
        const std::optional<Value> value = cache_.get(key);
        if (value.has_value()) {
            statistics_.recordHit();
        } else {
            statistics_.recordMiss();
        }
        return value;
    }

    void put(const Key &key, const Value &value) {
        cache_.put(key, value);
        statistics_.recordPut();
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
                throw std::invalid_argument("PUT requires a value");
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
    ThreadSafeLFUCache<Key, Value> cache_;
    CacheStatistics statistics_;
};

} // namespace lfu_cache_lld

#endif // LFU_CACHE_LLD_CORE_CACHESERVICE_H

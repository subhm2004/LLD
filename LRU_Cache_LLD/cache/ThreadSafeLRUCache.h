#ifndef LRU_CACHE_LLD_CACHE_THREADSAFELRUCACHE_H
#define LRU_CACHE_LLD_CACHE_THREADSAFELRUCACHE_H

#include <mutex>
#include <optional>

#include "../cache/ICache.h"
#include "../cache/LRUCacheCore.h"
#include "../config/CacheConfig.h"

namespace lru_cache_lld {

/**
 * Thread-safe decorator over LRUCacheCore.
 * Uses one mutex because get() also mutates recency order.
 */
template <typename Key, typename Value>
class ThreadSafeLRUCache : public ICache<Key, Value> {
public:
    explicit ThreadSafeLRUCache(const CacheConfig &config) : core_(config) {}

    std::optional<Value> get(const Key &key) override {
        std::lock_guard<std::mutex> lock(mutex_);
        return core_.get(key);
    }

    void put(const Key &key, const Value &value) override {
        std::lock_guard<std::mutex> lock(mutex_);
        core_.put(key, value);
    }

    bool contains(const Key &key) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return core_.contains(key);
    }

    bool remove(const Key &key) override {
        std::lock_guard<std::mutex> lock(mutex_);
        return core_.remove(key);
    }

    void clear() override {
        std::lock_guard<std::mutex> lock(mutex_);
        core_.clear();
    }

    size_t size() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return core_.size();
    }

    size_t capacity() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return core_.capacity();
    }

    bool didLastPutEvict() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return core_.didLastPutEvict();
    }

private:
    mutable std::mutex mutex_;
    LRUCacheCore<Key, Value> core_;
};

} // namespace lru_cache_lld

#endif // LRU_CACHE_LLD_CACHE_THREADSAFELRUCACHE_H

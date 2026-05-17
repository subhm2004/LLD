#ifndef LFU_CACHE_LLD_CACHE_THREADSAFELFUCACHE_H
#define LFU_CACHE_LLD_CACHE_THREADSAFELFUCACHE_H

#include <cstddef>
#include <mutex>
#include <optional>

#include "../cache/ICache.h"
#include "../cache/LFUCacheCore.h"
#include "../config/CacheConfig.h"
#include "../config/CppStandard.h"

namespace lfu_cache_lld {

template <typename Key, typename Value>
class ThreadSafeLFUCache : public ICache<Key, Value> {
public:
    explicit ThreadSafeLFUCache(const CacheConfig &config) : core_(config) {}

    ThreadSafeLFUCache(const ThreadSafeLFUCache &) = delete;
    ThreadSafeLFUCache &operator=(const ThreadSafeLFUCache &) = delete;
    ThreadSafeLFUCache(ThreadSafeLFUCache &&) = delete;
    ThreadSafeLFUCache &operator=(ThreadSafeLFUCache &&) = delete;

    [[nodiscard]] std::optional<Value> get(const Key &key) override {
        std::lock_guard<std::mutex> lock(mutex_);
        return core_.get(key);
    }

    void put(const Key &key, const Value &value) override {
        std::lock_guard<std::mutex> lock(mutex_);
        core_.put(key, value);
    }

    [[nodiscard]] bool contains(const Key &key) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return core_.contains(key);
    }

    [[nodiscard]] bool remove(const Key &key) override {
        std::lock_guard<std::mutex> lock(mutex_);
        return core_.remove(key);
    }

    void clear() override {
        std::lock_guard<std::mutex> lock(mutex_);
        core_.clear();
    }

    [[nodiscard]] std::size_t size() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return core_.size();
    }

    [[nodiscard]] std::size_t capacity() const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return core_.capacity();
    }

    [[nodiscard]] bool didLastPutEvict() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return core_.didLastPutEvict();
    }

    [[nodiscard]] int getFrequency(const Key &key) const {
        std::lock_guard<std::mutex> lock(mutex_);
        return core_.getFrequency(key);
    }

private:
    mutable std::mutex mutex_;
    LFUCacheCore<Key, Value> core_;
};

} // namespace lfu_cache_lld

#endif // LFU_CACHE_LLD_CACHE_THREADSAFELFUCACHE_H

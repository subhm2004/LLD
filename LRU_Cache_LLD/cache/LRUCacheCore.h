#ifndef LRU_CACHE_LLD_CACHE_LRUCACHECORE_H
#define LRU_CACHE_LLD_CACHE_LRUCACHECORE_H

#include <list>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <utility>

#include "../cache/ICache.h"
#include "../config/CacheConfig.h"
#include "../models/CacheEntry.h"

namespace lru_cache_lld {

/**
 * Single-threaded LRU cache.
 * Data structures:
 *   - Doubly linked list (std::list) for recency order (front = most recent)
 *   - Hash map from key -> list iterator for O(1) lookup
 */
template <typename Key, typename Value>
class LRUCacheCore : public ICache<Key, Value> {
public:
    explicit LRUCacheCore(const CacheConfig &config) : config_(config) {}

    std::optional<Value> get(const Key &key) override {
        auto iterator = lookup_.find(key);
        if (iterator == lookup_.end()) {
            return std::nullopt;
        }

        touch(iterator->second);
        return iterator->second->value;
    }

    void put(const Key &key, const Value &value) override {
        lastPutEvicted_ = false;

        auto iterator = lookup_.find(key);
        if (iterator != lookup_.end()) {
            iterator->second->value = value;
            touch(iterator->second);
            return;
        }

        order_.push_front(CacheEntry<Key, Value>(key, value));
        lookup_[key] = order_.begin();

        if (order_.size() > config_.getCapacity()) {
            evictLeastRecentlyUsed();
        }
    }

    bool contains(const Key &key) const override {
        return lookup_.find(key) != lookup_.end();
    }

    bool remove(const Key &key) override {
        auto iterator = lookup_.find(key);
        if (iterator == lookup_.end()) {
            return false;
        }

        order_.erase(iterator->second);
        lookup_.erase(iterator);
        return true;
    }

    void clear() override {
        order_.clear();
        lookup_.clear();
    }

    size_t size() const override { return lookup_.size(); }

    size_t capacity() const override { return config_.getCapacity(); }

    bool didLastPutEvict() const { return lastPutEvicted_; }

    void resetEvictionFlag() { lastPutEvicted_ = false; }

    std::list<CacheEntry<Key, Value>> getRecencyOrderSnapshot() const {
        return order_;
    }

private:
    using EntryList = std::list<CacheEntry<Key, Value>>;
    using EntryIterator = typename EntryList::iterator;

    void touch(EntryIterator iterator) {
        order_.splice(order_.begin(), order_, iterator);
    }

    void evictLeastRecentlyUsed() {
        if (order_.empty()) {
            return;
        }

        const Key &evictedKey = order_.back().key;
        lookup_.erase(evictedKey);
        order_.pop_back();
        lastPutEvicted_ = true;
    }

    CacheConfig config_;
    EntryList order_;
    std::unordered_map<Key, EntryIterator> lookup_;
    bool lastPutEvicted_ = false;
};

} // namespace lru_cache_lld

#endif // LRU_CACHE_LLD_CACHE_LRUCACHECORE_H

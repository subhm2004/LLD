#ifndef LFU_CACHE_LLD_CACHE_LFUCACHECORE_H
#define LFU_CACHE_LLD_CACHE_LFUCACHECORE_H

#include <cstddef>
#include <list>
#include <memory>
#include <optional>
#include <unordered_map>
#include <utility>

#include "../cache/ICache.h"
#include "../config/CacheConfig.h"
#include <bits/stdc++.h>
#include "../models/LFUNode.h"

namespace lfu_cache_lld {

/**
 * O(1) LFU cache (average) — C++17:
 *   std::optional, std::make_unique, inline variables, [[nodiscard]] via ICache
 */
template <typename Key, typename Value>
class LFUCacheCore : public ICache<Key, Value> {
public:
    using Node = LFUNode<Key, Value>;
    using NodePtr = std::unique_ptr<Node>;

    explicit LFUCacheCore(const CacheConfig &config) : config_(config), minFreq_(0) {}

    LFUCacheCore(const LFUCacheCore &) = delete;
    LFUCacheCore &operator=(const LFUCacheCore &) = delete;
    LFUCacheCore(LFUCacheCore &&) = delete;
    LFUCacheCore &operator=(LFUCacheCore &&) = delete;

    [[nodiscard]] std::optional<Value> get(const Key &key) override {
        const auto iterator = keyMap_.find(key);
        if (iterator == keyMap_.end()) {
            return std::nullopt;
        }

        increaseFrequency(iterator->second.get());
        return iterator->second->value;
    }

    void put(const Key &key, const Value &value) override {
        lastPutEvicted_ = false;

        const auto iterator = keyMap_.find(key);
        if (iterator != keyMap_.end()) {
            iterator->second->value = value;
            increaseFrequency(iterator->second.get());
            return;
        }

        if (keyMap_.size() >= config_.getCapacity()) {
            evictLeastFrequentlyUsed();
        }

        NodePtr node = std::make_unique<Node>(key, value);
        insertWithFrequencyOne(node.get());
        keyMap_.emplace(key, std::move(node));
    }

    [[nodiscard]] bool contains(const Key &key) const override {
        return keyMap_.find(key) != keyMap_.end();
    }

    [[nodiscard]] bool remove(const Key &key) override {
        const auto iterator = keyMap_.find(key);
        if (iterator == keyMap_.end()) {
            return false;
        }

        removeFromFrequencyBucket(iterator->second.get());
        keyMap_.erase(iterator);
        rebalanceMinFrequency();
        return true;
    }

    void clear() override {
        keyMap_.clear();
        freqBuckets_.clear();
        minFreq_ = 0;
        lastPutEvicted_ = false;
    }

    [[nodiscard]] std::size_t size() const override { return keyMap_.size(); }

    [[nodiscard]] std::size_t capacity() const override { return config_.getCapacity(); }

    [[nodiscard]] bool didLastPutEvict() const noexcept { return lastPutEvicted_; }

    [[nodiscard]] int getFrequency(const Key &key) const {
        const auto iterator = keyMap_.find(key);
        if (iterator == keyMap_.end()) {
            return -1;
        }
        return iterator->second->frequency;
    }

private:
    using BucketList = typename Node::BucketList;

    void insertWithFrequencyOne(Node *node) {
        node->frequency = 1;
        minFreq_ = 1;
        BucketList &bucket = freqBuckets_[1];
        bucket.push_front(node);
        node->bucketIterator = bucket.begin();
    }

    void increaseFrequency(Node *node) {
        const int currentFreq = node->frequency;
        BucketList &currentBucket = freqBuckets_[currentFreq];
        currentBucket.erase(node->bucketIterator);

        if (currentBucket.empty() && currentFreq == minFreq_) {
            ++minFreq_;
        }

        ++node->frequency;
        BucketList &newBucket = freqBuckets_[node->frequency];
        newBucket.push_front(node);
        node->bucketIterator = newBucket.begin();
    }

    void removeFromFrequencyBucket(Node *node) {
        BucketList &bucket = freqBuckets_[node->frequency];
        bucket.erase(node->bucketIterator);
        if (bucket.empty()) {
            freqBuckets_.erase(node->frequency);
        }
    }

    void rebalanceMinFrequency() {
        if (keyMap_.empty()) {
            minFreq_ = 0;
            return;
        }

        minFreq_ = 1;
        while (freqBuckets_.find(minFreq_) == freqBuckets_.end() ||
               freqBuckets_[minFreq_].empty()) {
            ++minFreq_;
        }
    }

    void evictLeastFrequentlyUsed() {
        if (freqBuckets_[minFreq_].empty()) {
            rebalanceMinFrequency();
        }

        BucketList &targetBucket = freqBuckets_[minFreq_];
        Node *victim = targetBucket.back();
        const Key evictedKey = victim->key;

        targetBucket.pop_back();
        if (targetBucket.empty()) {
            freqBuckets_.erase(minFreq_);
        }

        keyMap_.erase(evictedKey);
        lastPutEvicted_ = true;
        rebalanceMinFrequency();
    }

    CacheConfig config_;
    std::unordered_map<Key, NodePtr> keyMap_;
    std::unordered_map<int, BucketList> freqBuckets_;
    int minFreq_{0};
    bool lastPutEvicted_{false};
};

} // namespace lfu_cache_lld

#endif // LFU_CACHE_LLD_CACHE_LFUCACHECORE_H

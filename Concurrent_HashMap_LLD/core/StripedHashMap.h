#ifndef CONCURRENT_HASHMAP_LLD_CORE_STRIPEDHASHMAP_H
#define CONCURRENT_HASHMAP_LLD_CORE_STRIPEDHASHMAP_H

#include <functional>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "IConcurrentMap.h"
#include "../stats/MapStatistics.h"

namespace concurrent_hashmap_lld {

/**
 * Approach 2 — Fine-grained lock striping (C++17).
 * hash(key) % num_stripes → per-stripe mutex + local unordered_map.
 * Different keys in different stripes can run in parallel.
 */
class StripedHashMap : public IConcurrentMap {
public:
    explicit StripedHashMap(size_t num_stripes = 16)
        : stripes_(num_stripes > 0 ? num_stripes : 16) {
        buckets_.resize(stripes_);
        locks_.reserve(stripes_);
        for (size_t i = 0; i < stripes_; ++i) {
            locks_.emplace_back(std::make_unique<std::mutex>());
        }
    }

    void put(const std::string& key, const std::string& value) override {
        size_t idx = stripeIndex(key);
        std::lock_guard<std::mutex> lock(*locks_[idx]);
        buckets_[idx][key] = value;
        stats_.recordPut();
    }

    std::optional<std::string> get(const std::string& key) const override {
        size_t idx = stripeIndex(key);
        std::lock_guard<std::mutex> lock(*locks_[idx]);
        const auto& bucket = buckets_[idx];
        auto it = bucket.find(key);
        if (it == bucket.end()) {
            stats_.recordGet(false);
            return std::nullopt;
        }
        stats_.recordGet(true);
        return it->second;
    }

    bool remove(const std::string& key) override {
        size_t idx = stripeIndex(key);
        std::lock_guard<std::mutex> lock(*locks_[idx]);
        bool erased = buckets_[idx].erase(key) > 0;
        if (erased) {
            stats_.recordRemove();
        }
        return erased;
    }

    bool contains(const std::string& key) const override {
        size_t idx = stripeIndex(key);
        std::lock_guard<std::mutex> lock(*locks_[idx]);
        return buckets_[idx].find(key) != buckets_[idx].end();
    }

    size_t size() const override {
        size_t total = 0;
        for (size_t i = 0; i < stripes_; ++i) {
            std::lock_guard<std::mutex> lock(*locks_[i]);
            total += buckets_[i].size();
        }
        return total;
    }

    const char* strategyName() const override { return "LockStriping (fine-grained)"; }

    size_t numStripes() const { return stripes_; }

    MapStatistics& statistics() { return stats_; }
    const MapStatistics& statistics() const { return stats_; }

private:
    size_t stripeIndex(const std::string& key) const {
        std::hash<std::string> hasher;
        return hasher(key) % stripes_;
    }

    size_t stripes_;
    mutable std::vector<std::unordered_map<std::string, std::string>> buckets_;
    mutable std::vector<std::unique_ptr<std::mutex>> locks_;
    mutable MapStatistics stats_;
};

}  // namespace concurrent_hashmap_lld

#endif  // CONCURRENT_HASHMAP_LLD_CORE_STRIPEDHASHMAP_H

#ifndef CONCURRENT_HASHMAP_LLD_CORE_COARSEGRAINEDHASHMAP_H
#define CONCURRENT_HASHMAP_LLD_CORE_COARSEGRAINEDHASHMAP_H

#include <cstddef>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

#include "IConcurrentMap.h"  // needs C++17 (<optional>)
#include "../stats/MapStatistics.h"

namespace concurrent_hashmap_lld {

/**
 * Approach 1 — Coarse-grained: one mutex for entire map.
 * Simple, correct; low parallelism under contention.
 */
class CoarseGrainedHashMap : public IConcurrentMap {
public:
    void put(const std::string& key, const std::string& value) override {
        std::lock_guard<std::mutex> lock(mtx_);
        store_[key] = value;
        stats_.recordPut();
    }

    std::optional<std::string> get(const std::string& key) const override {
        std::lock_guard<std::mutex> lock(mtx_);
        auto it = store_.find(key);
        if (it == store_.end()) {
            stats_.recordGet(false);
            return std::nullopt;
        }
        stats_.recordGet(true);
        return it->second;
    }

    bool remove(const std::string& key) override {
        std::lock_guard<std::mutex> lock(mtx_);
        bool erased = store_.erase(key) > 0;
        if (erased) {
            stats_.recordRemove();
        }
        return erased;
    }

    bool contains(const std::string& key) const override {
        std::lock_guard<std::mutex> lock(mtx_);
        return store_.find(key) != store_.end();
    }

    size_t size() const override {
        std::lock_guard<std::mutex> lock(mtx_);
        return store_.size();
    }

    const char* strategyName() const override { return "CoarseGrained (1 mutex)"; }

    MapStatistics& statistics() { return stats_; }
    const MapStatistics& statistics() const { return stats_; }

private:
    mutable std::mutex mtx_;
    std::unordered_map<std::string, std::string> store_;
    mutable MapStatistics stats_;
};

}  // namespace concurrent_hashmap_lld

#endif  // CONCURRENT_HASHMAP_LLD_CORE_COARSEGRAINEDHASHMAP_H

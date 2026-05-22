#ifndef THREAD_SAFE_TTL_CACHE_LLD_CORE_THREADSAFETTLCACHE_H
#define THREAD_SAFE_TTL_CACHE_LLD_CORE_THREADSAFETTLCACHE_H

#include <chrono>
#include <iostream>
#include <map>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>

#include "../models/CacheEntry.h"
#include "../stats/CacheStatistics.h"

namespace ttl_cache_lld {

/**
 * Thread-safe in-memory cache with per-key TTL.
 * C++17: std::shared_mutex, std::optional, structured bindings in loops.
 *
 * Concurrency handling:
 * - Stale read: get() checks expiry under lock; expired => miss + lazy erase
 * - Put vs cleanup: both use exclusive lock path for map mutation
 * - Concurrent put: serialized by unique_lock
 * - Read during modify: shared_lock for valid entries; writers exclusive
 */
class ThreadSafeTTLCache {
public:
    explicit ThreadSafeTTLCache(size_t max_entries = 1000) : max_entries_(max_entries) {}

    void put(const std::string& key, const std::string& value, int ttl_seconds) {
        auto expires_at = std::chrono::steady_clock::now() +
                          std::chrono::seconds(ttl_seconds);

        std::unique_lock<std::shared_mutex> lock(mtx_);
        evictIfNeededLocked();
        store_[key] = CacheEntry{value, expires_at};
        stats_.recordPut();
    }

    std::optional<std::string> get(const std::string& key) {
        {
            std::shared_lock<std::shared_mutex> lock(mtx_);
            auto it = store_.find(key);
            if (it == store_.end()) {
                stats_.recordMiss();
                return std::nullopt;
            }
            if (!it->second.isExpired()) {
                stats_.recordHit();
                return it->second.value;
            }
            stats_.recordExpiredOnGet();
        }

        return lazyEraseAndMiss(key);
    }

    bool contains(const std::string& key) const {
        std::shared_lock<std::shared_mutex> lock(mtx_);
        auto it = store_.find(key);
        return it != store_.end() && !it->second.isExpired();
    }

    bool remove(const std::string& key) {
        std::unique_lock<std::shared_mutex> lock(mtx_);
        return store_.erase(key) > 0;
    }

    int cleanupExpired() {
        std::unique_lock<std::shared_mutex> lock(mtx_);
        int removed = 0;
        for (auto it = store_.begin(); it != store_.end();) {
            if (it->second.isExpired()) {
                it = store_.erase(it);
                ++removed;
            } else {
                ++it;
            }
        }
        stats_.recordCleanupSweep(removed);
        return removed;
    }

    size_t size() const {
        std::shared_lock<std::shared_mutex> lock(mtx_);
        size_t count = 0;
        for (const auto& [k, entry] : store_) {
            (void)k;
            if (!entry.isExpired()) {
                ++count;
            }
        }
        return count;
    }

    size_t totalEntriesIncludingExpired() const {
        std::shared_lock<std::shared_mutex> lock(mtx_);
        return store_.size();
    }

    size_t maxEntries() const { return max_entries_; }

    const CacheStatistics& statistics() const { return stats_; }

    void printEntries() const {
        std::shared_lock<std::shared_mutex> lock(mtx_);
        std::cout << "\n--- Cache entries ---\n";
        if (store_.empty()) {
            std::cout << "(empty)\n";
            return;
        }
        for (const auto& [key, entry] : store_) {
            std::cout << "  " << key << " => \"" << entry.value << "\""
                      << (entry.isExpired() ? " [EXPIRED]" : "")
                      << " remaining_ms=" << entry.remainingMs() << "\n";
        }
    }

private:
    std::optional<std::string> lazyEraseAndMiss(const std::string& key) {
        std::unique_lock<std::shared_mutex> lock(mtx_);
        auto it = store_.find(key);
        if (it == store_.end()) {
            stats_.recordMiss();
            return std::nullopt;
        }
        if (it->second.isExpired()) {
            store_.erase(it);
            stats_.recordLazyRemoval();
            stats_.recordMiss();
            return std::nullopt;
        }
        stats_.recordHit();
        return it->second.value;
    }

    void evictIfNeededLocked() {
        if (store_.size() < max_entries_) {
            return;
        }
        cleanupExpiredLocked();
        while (store_.size() >= max_entries_) {
            auto oldest = store_.end();
            for (auto it = store_.begin(); it != store_.end(); ++it) {
                if (oldest == store_.end() ||
                    it->second.expires_at < oldest->second.expires_at) {
                    oldest = it;
                }
            }
            if (oldest == store_.end()) {
                break;
            }
            store_.erase(oldest);
        }
    }

    void cleanupExpiredLocked() {
        for (auto it = store_.begin(); it != store_.end();) {
            if (it->second.isExpired()) {
                it = store_.erase(it);
            } else {
                ++it;
            }
        }
    }

    size_t max_entries_;
    mutable std::shared_mutex mtx_;
    std::map<std::string, CacheEntry> store_;
    CacheStatistics stats_;
};

}  // namespace ttl_cache_lld

#endif  // THREAD_SAFE_TTL_CACHE_LLD_CORE_THREADSAFETTLCACHE_H

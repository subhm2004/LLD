// ============================================================================
//  cache/ThreadSafeLRUCache.h  —  LRU core ke upar THREAD-SAFE wrapper
//                                 (Decorator / Proxy Pattern)
// ----------------------------------------------------------------------------
//  Ye class andar ek `LRUCacheCore` rakhti hai aur uske HAR method ko ek mutex
//  ke peeche wrap karti hai. Bahar se ye bhi ICache hi lagti hai — same interface,
//  bas "thread-safe" super-power add.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ DECORATOR PATTERN — asli object badle bina uske upar feature add     │
//  │                                                                          │
//  │  LRUCacheCore ko locks ki khabar nahi — wo sirf LRU algorithm janta hai   │
//  │  (Single Responsibility). Thread-safety ek ALAG concern hai, jise ye      │
//  │  wrapper add karta hai. Single-thread me core seedha, multi-thread me     │
//  │  wrapper — ek hi algorithm dono jagah reuse.                              │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `get()` bhi LOCK leta hai — kyunki get() RECENCY BADALTA hai          │
//  │                                                                          │
//  │  LRU me get() entry ko front (MRU) pe splice karta hai — yaani cache ki   │
//  │  order MUTATE hoti hai. To get bhi ek WRITE hai! Isi liye get par bhi     │
//  │  full mutex chahiye, aur `shared_mutex` (parallel reads) yahan seedhe     │
//  │  kaam nahi karta. (Read-only "peek" API — jo recency na badle — ek        │
//  │  extension idea hai; tab shared_mutex use ho sakta.)                     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `std::lock_guard` RAII: scope me lock, scope se nikalte hi auto-unlock.
//  ⚠ Ek hi mutex poore cache pe = simple + SAHI, par high-concurrency bottleneck.
//     Scale ke liye "sharded locks" (key hash se N cache+lock) — interview extension.
// ============================================================================
#ifndef LRU_CACHE_LLD_CACHE_THREADSAFELRUCACHE_H
#define LRU_CACHE_LLD_CACHE_THREADSAFELRUCACHE_H

#include <mutex>
#include <optional>

#include "../cache/ICache.h"
#include "../cache/LRUCacheCore.h"
#include "../config/CacheConfig.h"

namespace lru_cache_lld {

template <typename Key, typename Value>
class ThreadSafeLRUCache : public ICache<Key, Value> {
public:
    explicit ThreadSafeLRUCache(const CacheConfig &config) : core_(config) {}

    // ⭐ Har method ka pattern SAME: lock lo -> core ko delegate -> auto-unlock.

    std::optional<Value> get(const Key &key) override {
        std::lock_guard<std::mutex> lock(mutex_); // get bhi write hai (recency badalta)
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
    // `mutable` — const methods (size/capacity/contains) me bhi lock le sakein.
    mutable std::mutex mutex_;
    LRUCacheCore<Key, Value> core_; // asli algorithm (locks se anjaan)
};

} // namespace lru_cache_lld

#endif // LRU_CACHE_LLD_CACHE_THREADSAFELRUCACHE_H

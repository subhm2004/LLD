// ============================================================================
//  cache/ThreadSafeLFUCache.h  —  LFU core ke upar THREAD-SAFE wrapper
//                                 (Decorator / Proxy Pattern)
// ----------------------------------------------------------------------------
//  Ye class andar ek `LFUCacheCore` rakhti hai aur uske HAR method ko ek mutex
//  lock ke peeche wrap kar deti hai. Bahar se ye bhi ICache hi lagti hai —
//  same interface, bas "thread-safe" super-power add ho gayi.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ DECORATOR PATTERN — asli object ko badle bina uske upar feature add   │
//  │                                                                          │
//  │  LFUCacheCore ko locks ki koi khabar nahi — wo sirf LFU algorithm janta   │
//  │  hai (Single Responsibility). Thread-safety ek ALAG concern hai, jise ye  │
//  │  wrapper add karta hai. Fayda:                                            │
//  │    - Single-thread me core ko seedha use karo (bina lock overhead ke)     │
//  │    - Multi-thread me is wrapper se use karo                               │
//  │  Dono cases ek hi algorithm code share karte hain — koi duplication nahi. │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `get()` bhi LOCK leta hai — ye chaunkane wali baat hai               │
//  │                                                                          │
//  │  Aam soch: "read (get) to safe hoga, lock kyun?" Par LFU me get() cache   │
//  │  ko BADALTA hai — frequency badhata hai, node ko doosre bucket me shift   │
//  │  karta hai. Yaani get bhi ek WRITE hai! Isi liye get par bhi full mutex   │
//  │  chahiye. (Isi wajah se yahan `shared_mutex` se read-parallelism nahi     │
//  │  mil sakta — har get bhi exclusive lock leta hai.)                       │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `std::lock_guard` RAII hai: scope me aate hi lock, scope se nikalte hi
//     (return/exception) apne aap unlock. Unlock bhoolne ka dar hi nahi.
//
//  ⚠ Ek hi mutex poore cache pe = simple aur SAHI (correctness first), par
//     high concurrency me bottleneck. Scale ke liye "sharded locks" (key hash
//     se N alag cache+lock) hote hain — interview extension.
// ============================================================================
#ifndef LFU_CACHE_LLD_CACHE_THREADSAFELFUCACHE_H
#define LFU_CACHE_LLD_CACHE_THREADSAFELFUCACHE_H

#include <cstddef>
#include <mutex>
#include <optional>

#include "../cache/ICache.h"
#include "../cache/LFUCacheCore.h"
#include "../config/CacheConfig.h"

namespace lfu_cache_lld {

template <typename Key, typename Value>
class ThreadSafeLFUCache : public ICache<Key, Value> {
public:
    explicit ThreadSafeLFUCache(const CacheConfig &config) : core_(config) {}

    // Copy/move band — mutex copy nahi hota, aur do wrapper ek core share karein
    // to race. Isliye non-copyable/movable.
    ThreadSafeLFUCache(const ThreadSafeLFUCache &) = delete;
    ThreadSafeLFUCache &operator=(const ThreadSafeLFUCache &) = delete;
    ThreadSafeLFUCache(ThreadSafeLFUCache &&) = delete;
    ThreadSafeLFUCache &operator=(ThreadSafeLFUCache &&) = delete;

    // ⭐ Har method ka pattern SAME hai: lock lo -> core ko delegate karo ->
    //    scope khatam pe auto-unlock. Bas.

    [[nodiscard]] std::optional<Value> get(const Key &key) override {
        std::lock_guard<std::mutex> lock(mutex_); // get bhi write hai (freq badalta)
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
    // ⭐ `mutable` — const methods (size/capacity/contains) me bhi lock le sakein.
    // Lock lena object ki "logical state" nahi badalta, isliye mutable sahi hai.
    mutable std::mutex mutex_;
    LFUCacheCore<Key, Value> core_; // asli algorithm (locks se anjaan)
};

} // namespace lfu_cache_lld

#endif // LFU_CACHE_LLD_CACHE_THREADSAFELFUCACHE_H

// ============================================================================
//  cache/LFUCacheCore.h  —  O(1) LFU cache ka ASLI dimaag (single-threaded)
// ----------------------------------------------------------------------------
//  LFU = "Least Frequently Used". Jab jagah bhar jaaye, to wo item nikaalo jo
//  SABSE KAM BAAR use hua. Agar do items ki frequency same ho, to un dono me se
//  jo sabse purana (least-recently-used) hai wo nikaalo (tie-break = LRU).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ O(1) KAISE? — 3 data structures ka jaadu                             │
//  │                                                                          │
//  │  1. keyMap_      : key -> Node          (O(1) lookup — key kahan hai)     │
//  │  2. freqBuckets_ : frequency -> list of nodes (us freq wale saare nodes)  │
//  │  3. minFreq_     : abhi ki sabse choti frequency (evict yahin se hoga)    │
//  │                                                                          │
//  │  Har frequency ka apna ek "bucket" (list) hai. Ek node apni frequency ke  │
//  │  bucket me rehta hai. Frequency badhi -> node ko agle bucket me shift.    │
//  │  Evict karna ho -> minFreq_ wale bucket ke PEECHE (back) se nikaalo.      │
//  │                                                                          │
//  │      freqBuckets_:                                                        │
//  │        [freq 1] -> D <-> C        (front = naya, back = purana = LRU)     │
//  │        [freq 2] -> ...                                                    │
//  │        [freq 4] -> A                                                      │
//  │      minFreq_ = 1  -> evict karna ho to freq-1 bucket ki back (C)         │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ OWNERSHIP — kaun kis node ka MAALIK hai (memory ka asli sawaal)      │
//  │                                                                          │
//  │  keyMap_ nodes ko `unique_ptr` se OWN karta hai (asli maalik).           │
//  │  freqBuckets_ ki list sirf RAW POINTER (Node*) rakhti hai — sirf         │
//  │  "point" karti hai, own nahi karti.                                      │
//  │                                                                          │
//  │  Kyun? Ek node do jagah hai (keyMap me + kisi bucket me). Agar dono own   │
//  │  karte (do unique_ptr) to double-free crash. Isliye: EK maalik (keyMap),  │
//  │  doosra sirf reference (bucket). keyMap se erase = node destroy = list ka │
//  │  raw pointer dangling... isliye ORDER important: pehle bucket se nikaalo, │
//  │  PHIR keyMap se erase karo. (remove/evict me yahi order dekho.)          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Har Node apna `bucketIterator` rakhta hai — us list me apni jagah ka
//     "pata". Isse list se O(1) me erase ho jaata (poori list scan nahi karni).
//     Yahi "iterator ko node me store karna" wali trick LFU/LRU dono ka core hai.
//
//  📌 Ye class SINGLE-THREADED hai (koi lock nahi). Thread-safety alag layer
//     (ThreadSafeLFUCache) deta hai — SRP: algorithm alag, locking alag.
// ============================================================================
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
#include "../models/LFUNode.h"

namespace lfu_cache_lld {

template <typename Key, typename Value>
class LFUCacheCore : public ICache<Key, Value> {
public:
    using Node = LFUNode<Key, Value>;
    using NodePtr = std::unique_ptr<Node>; // keyMap node ko OWN karta hai

    explicit LFUCacheCore(const CacheConfig &config) : config_(config), minFreq_(0) {}

    // Copy/move band — cache ek jagah ka single owner hai; raw pointers (bucket
    // iterators) copy hone par dangling ho jaate. Isliye non-copyable/movable.
    LFUCacheCore(const LFUCacheCore &) = delete;
    LFUCacheCore &operator=(const LFUCacheCore &) = delete;
    LFUCacheCore(LFUCacheCore &&) = delete;
    LFUCacheCore &operator=(LFUCacheCore &&) = delete;

    // ---- GET: mila to frequency +1 aur value do; nahi mila to nullopt -------
    [[nodiscard]] std::optional<Value> get(const Key &key) override {
        const auto iterator = keyMap_.find(key);
        if (iterator == keyMap_.end()) {
            return std::nullopt; // MISS
        }
        // HIT -> ye access hai, frequency badhao (item aur "hot" ho gaya)
        increaseFrequency(iterator->second.get());
        return iterator->second->value;
    }

    // ---- PUT: naya insert ya purana update -----------------------------------
    void put(const Key &key, const Value &value) override {
        lastPutEvicted_ = false; // is put me eviction hui? (stats ke liye reset)

        const auto iterator = keyMap_.find(key);
        if (iterator != keyMap_.end()) {
            // Key pehle se hai -> value update + frequency +1 (update bhi access hai)
            iterator->second->value = value;
            increaseFrequency(iterator->second.get());
            return;
        }

        // Nayi key. Jagah nahi hai? -> pehle sabse kam-frequency wala nikaalo.
        if (keyMap_.size() >= config_.getCapacity()) {
            evictLeastFrequentlyUsed();
        }

        // Naya node banao (frequency 1 se shuru) aur keyMap me daalo (ownership).
        NodePtr node = std::make_unique<Node>(key, value);
        insertWithFrequencyOne(node.get()); // pehle bucket me raw pointer
        keyMap_.emplace(key, std::move(node)); // phir ownership keyMap ko
    }

    // ---- CONTAINS: sirf check (frequency ko HAATH nahi lagata) ---------------
    [[nodiscard]] bool contains(const Key &key) const override {
        return keyMap_.find(key) != keyMap_.end();
    }

    // ---- REMOVE: key hatao (pehle bucket se, phir keyMap se — order!) --------
    [[nodiscard]] bool remove(const Key &key) override {
        const auto iterator = keyMap_.find(key);
        if (iterator == keyMap_.end()) {
            return false;
        }
        removeFromFrequencyBucket(iterator->second.get()); // pehle bucket (raw ptr valid rahe)
        keyMap_.erase(iterator);                            // phir asli node destroy
        rebalanceMinFrequency();                            // minFreq_ shayad badal gaya
        return true;
    }

    void clear() override {
        keyMap_.clear();     // saare nodes destroy (unique_ptr)
        freqBuckets_.clear();
        minFreq_ = 0;
        lastPutEvicted_ = false;
    }

    [[nodiscard]] std::size_t size() const override { return keyMap_.size(); }
    [[nodiscard]] std::size_t capacity() const override { return config_.getCapacity(); }

    // Kya abhi ke put me eviction hui? (CacheService stats ke liye poochta hai)
    [[nodiscard]] bool didLastPutEvict() const noexcept { return lastPutEvicted_; }

    // Debug/demo helper — kisi key ki current frequency (nahi hai to -1).
    [[nodiscard]] int getFrequency(const Key &key) const {
        const auto iterator = keyMap_.find(key);
        if (iterator == keyMap_.end()) {
            return -1;
        }
        return iterator->second->frequency;
    }

private:
    using BucketList = typename Node::BucketList; // std::list<Node*>

    // ---- Naya node frequency-1 bucket ke FRONT me daalo ---------------------
    void insertWithFrequencyOne(Node *node) {
        node->frequency = 1;
        minFreq_ = 1; // naya node hamesha freq 1 = nayi minimum
        BucketList &bucket = freqBuckets_[1];
        bucket.push_front(node);            // front = "abhi aaya" (MRU side)
        node->bucketIterator = bucket.begin(); // apni jagah yaad rakho (O(1) erase ke liye)
    }

    // ---- Node ki frequency +1: current bucket se agle bucket me shift -------
    void increaseFrequency(Node *node) {
        const int currentFreq = node->frequency;
        BucketList &currentBucket = freqBuckets_[currentFreq];
        currentBucket.erase(node->bucketIterator); // O(1) — iterator stored tha

        // ⭐ Agar current bucket khaali ho gaya AUR wahi minFreq_ tha, to ab
        // minimum frequency ek badh gayi (is freq pe koi node nahi bacha).
        if (currentBucket.empty() && currentFreq == minFreq_) {
            ++minFreq_;
        }

        ++node->frequency;
        BucketList &newBucket = freqBuckets_[node->frequency];
        newBucket.push_front(node);            // naye (higher) bucket ke front me
        node->bucketIterator = newBucket.begin();
    }

    // ---- Node ko uske bucket se nikaalo (remove ke liye) --------------------
    void removeFromFrequencyBucket(Node *node) {
        BucketList &bucket = freqBuckets_[node->frequency];
        bucket.erase(node->bucketIterator);
        if (bucket.empty()) {
            freqBuckets_.erase(node->frequency); // khaali bucket map se hata do (safai)
        }
    }

    // ---- minFreq_ ko dobara sahi karo (remove/evict ke baad) ----------------
    void rebalanceMinFrequency() {
        if (keyMap_.empty()) {
            minFreq_ = 0; // cache khaali -> koi min nahi
            return;
        }
        // 1 se upar chalo jab tak pehla non-empty bucket na mile.
        // (keyMap non-empty hai to koi na koi bucket zaroor milega — loop rukega.)
        minFreq_ = 1;
        while (freqBuckets_.find(minFreq_) == freqBuckets_.end() ||
               freqBuckets_[minFreq_].empty()) {
            ++minFreq_;
        }
    }

    // ---- EVICTION: minFreq_ bucket ke BACK (LRU tie-break) se nikaalo -------
    void evictLeastFrequentlyUsed() {
        if (freqBuckets_[minFreq_].empty()) {
            rebalanceMinFrequency(); // safety: minFreq_ galat ho to theek karo
        }

        BucketList &targetBucket = freqBuckets_[minFreq_];
        // ⭐ back() = us frequency ka SABSE PURANA node = LRU within min-freq.
        // (front pe naye aate hain, to back pe sabse purana rehta hai.)
        Node *victim = targetBucket.back();
        const Key evictedKey = victim->key;

        targetBucket.pop_back();
        if (targetBucket.empty()) {
            freqBuckets_.erase(minFreq_);
        }

        keyMap_.erase(evictedKey); // asli node destroy (unique_ptr) — victim ab invalid
        lastPutEvicted_ = true;    // stats: is put ne kisi ko nikaala
        rebalanceMinFrequency();
    }

    CacheConfig config_;
    std::unordered_map<Key, NodePtr> keyMap_;            // key -> node (OWNER)
    std::unordered_map<int, BucketList> freqBuckets_;    // freq -> nodes (raw ptrs)
    int minFreq_{0};                                     // evict pointer
    bool lastPutEvicted_{false};                         // last put me eviction hui?
};

} // namespace lfu_cache_lld

#endif // LFU_CACHE_LLD_CACHE_LFUCACHECORE_H

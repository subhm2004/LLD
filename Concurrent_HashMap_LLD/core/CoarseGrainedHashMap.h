// ============================================================================
//  core/CoarseGrainedHashMap.h  —  Approach 1: ek hi mutex poore map pe
// ----------------------------------------------------------------------------
//  Sabse simple thread-safe map: ek `unordered_map` + ek `mutex`. HAR operation
//  (put/get/remove/...) pehle wo ek mutex lock karta hai, phir kaam karta hai.
//  "Coarse-grained" = mota/bada lock (ek hi lock sab kuch guard karta hai).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ CORRECT hai, par PARALLELISM kam                                       │
//  │                                                                          │
//  │  Ek hi mutex ka matlab: ek waqt me SIRF EK thread map ko chhu sakta hai — │
//  │  chahe wo alag-alag keys pe kaam kar rahe hon. 100 threads bhi lag jaayein│
//  │  to bhi wo ek-ek karke (serialize) chalenge. Ye SAHI hai (koi corruption  │
//  │  nahi) par TEZ nahi (contention zyada -> threads ek doosre ka wait karte).│
//  │                                                                          │
//  │  ✅ Kab use karein: simple code chahiye, ya load kam hai (thoda contention)│
//  │  ❌ Kab nahi: high concurrency, alag-alag keys pe bahut parallel kaam     │
//  │     (tab StripedHashMap behtar — wo alag keys ko parallel chalne deta).   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `std::lock_guard` (RAII): scope me aate hi lock, scope se nikalte hi (return
//     ya exception) auto-unlock. Unlock bhoolne ka dar nahi.
//
//  ⭐ `mutable std::mutex mtx_` — `mutable` isliye taaki CONST methods (get,
//     contains, size) bhi lock le sakein. Lock lena object ki "logical state"
//     nahi badalta, isliye const method me mutable member modify karna sahi hai.
//
//  📌 Ye class copy nahi ho sakti (mutex + atomic stats dono non-copyable hain) —
//     jo accha hai (galti se copy = alag lock = race). Explicit = delete ki
//     zaroorat nahi, members hi rok dete hain.
// ============================================================================
#ifndef CONCURRENT_HASHMAP_LLD_CORE_COARSEGRAINEDHASHMAP_H
#define CONCURRENT_HASHMAP_LLD_CORE_COARSEGRAINEDHASHMAP_H

#include <cstddef>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>

#include "IConcurrentMap.h"
#include "../stats/MapStatistics.h"

namespace concurrent_hashmap_lld {

class CoarseGrainedHashMap : public IConcurrentMap {
public:
    void put(const std::string& key, const std::string& value) override {
        std::lock_guard<std::mutex> lock(mtx_); // poore map ka ek lock
        store_[key] = value;                    // insert ya update (last-writer-wins)
        stats_.recordPut();
    }

    std::optional<std::string> get(const std::string& key) const override {
        std::lock_guard<std::mutex> lock(mtx_);
        auto it = store_.find(key);
        if (it == store_.end()) {
            stats_.recordGet(false); // miss
            return std::nullopt;
        }
        stats_.recordGet(true);      // hit
        return it->second;
    }

    bool remove(const std::string& key) override {
        std::lock_guard<std::mutex> lock(mtx_);
        bool erased = store_.erase(key) > 0; // erase count 0 (nahi tha) ya 1 (hata diya)
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
    mutable std::mutex mtx_;                                  // ⭐ EK lock, sab kuch guard karta hai
    std::unordered_map<std::string, std::string> store_;     // asli data
    mutable MapStatistics stats_;                            // metrics (atomic, alag concern)
};

}  // namespace concurrent_hashmap_lld

#endif  // CONCURRENT_HASHMAP_LLD_CORE_COARSEGRAINEDHASHMAP_H

// ============================================================================
//  utils/ConcurrencyHelpers.h  —  Threads ko coordinate karne ke 2 tools
// ----------------------------------------------------------------------------
//  Stress-test me kai threads ek saath chalte hain. Unhe "ek saath shuru karo"
//  aur "sab khatam hone ka wait karo" — ye 2 kaam ke liye 2 chhote helpers.
//  (Java ke CyclicBarrier / CountDownLatch se prerit.)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  1. CyclicBarrier — "sab pahunch jaayein, tabhi sab aage badhein"        │
//  │     Har thread arriveAndWait() pe RUK jaata hai. Aakhri thread pahunchte  │
//  │     hi (waiting_ == 0) sab ek saath chhoot jaate hain. Fayda: stress test │
//  │     me sab threads THEEK EK saath shuru — "race" fair rehti hai.         │
//  │                                                                          │
//  │  2. CountDownLatch — "gin-gin ke 0 hone ka wait karo"                    │
//  │     Counter N se shuru. Har thread khatam hote hi countDown() (N--).      │
//  │     Main thread await() pe ruka rehta jab tak count 0 na ho — "sab N      │
//  │     threads poore ho gaye" ka signal.                                    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `condition_variable` pattern (dono me): mutex lo, `wait(lock, predicate)` —
//     thread SO jaata hai (CPU waste nahi) jab tak notify na aaye AUR predicate
//     true na ho. Predicate-form "spurious wakeups" (bina wajah jaagna) se bachata.
//
//  ⚠ Ye CyclicBarrier asal me ONE-SHOT hai (naam ke bawajood) — `released_` reset
//     nahi hota. Is demo me ek hi baar chahiye, isliye theek hai.
// ============================================================================
#ifndef LRU_CACHE_LLD_UTILS_CONCURRENCYHELPERS_H
#define LRU_CACHE_LLD_UTILS_CONCURRENCYHELPERS_H

#include <condition_variable>
#include <mutex>

namespace lru_cache_lld {

// ---- Sab threads ek saath start karne ke liye ------------------------------
class CyclicBarrier {
public:
    explicit CyclicBarrier(int participantCount) : waiting_(participantCount) {}

    void arriveAndWait() {
        std::unique_lock<std::mutex> lock(mutex_);
        if (--waiting_ == 0) {
            // Main aakhri thread -> sabko jagao
            released_ = true;
            condition_.notify_all();
            return;
        }
        // Baaki threads baaki hain -> so jao jab tak released_ na ho
        condition_.wait(lock, [this] { return released_; });
    }

private:
    int waiting_;                      // kitne pahunchne baaki
    bool released_ = false;            // sab pahunch gaye?
    std::mutex mutex_;
    std::condition_variable condition_;
};

// ---- Sab threads ke khatam hone ka wait karne ke liye ----------------------
class CountDownLatch {
public:
    explicit CountDownLatch(int count) : count_(count) {}

    // Ek thread ne kaam khatam kiya -> counter ghatao; 0 ho gaya to sabko jagao.
    void countDown() {
        std::unique_lock<std::mutex> lock(mutex_);
        if (count_ > 0) {
            --count_;
        }
        if (count_ == 0) {
            condition_.notify_all();
        }
    }

    // Jab tak count 0 na ho, yahin ruko (CPU waste kiye bina).
    void await() {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [this] { return count_ == 0; });
    }

private:
    int count_;
    std::mutex mutex_;
    std::condition_variable condition_;
};

} // namespace lru_cache_lld

#endif // LRU_CACHE_LLD_UTILS_CONCURRENCYHELPERS_H

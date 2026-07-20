// ============================================================================
//  utils/ConcurrencyHelpers.h  —  Threads ko coordinate karne ke 2 tools
// ----------------------------------------------------------------------------
//  Stress-test me kai threads ek saath chalte hain. Unhe "ek saath shuru karo"
//  aur "sab khatam hone ka wait karo" — ye 2 kaam ke liye 2 chhote helpers.
//  (Java ke CyclicBarrier / CountDownLatch se prerit.)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  1. CyclicBarrier — "sab pahunch jaayein, tabhi sab aage badhein"        │
//  │                                                                          │
//  │  Har thread arriveAndWait() bulata hai aur RUK jaata hai. Jab AAKHRI      │
//  │  thread pahunchta hai (waiting_ == 0), to sab ek saath chhoot jaate hain. │
//  │  Fayda: stress test me sab threads THEEK EK saath kaam shuru karein —     │
//  │  koi pehle warm-up na kar le. "Race" fair rehti hai.                     │
//  │                                                                          │
//  │  2. CountDownLatch — "gin-gin ke 0 hone ka wait karo"                    │
//  │                                                                          │
//  │  Ek counter N se shuru. Har thread khatam hote hi countDown() (N--).      │
//  │  Main thread await() pe ruka rehta hai jab tak count 0 na ho. Yaani       │
//  │  "sab N threads ka kaam poora ho gaya" ka signal.                        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `condition_variable` ka pattern (dono me): mutex lo, condition check karo,
//     `wait(lock, predicate)` — ye thread ko SO jaata hai (CPU waste nahi) jab
//     tak koi `notify_all()` na kare AUR predicate true na ho. Predicate wala
//     form "spurious wakeups" (bina wajah jaag jaana) se bachata hai.
//
//  ⚠ Ye CyclicBarrier asal me ONE-SHOT hai (naam ke bawajood) — `released_`
//     ek baar true hone ke baad reset nahi hota, to dobara use nahi kar sakte.
//     Is demo me ek hi baar chahiye, isliye theek hai. Sacha cyclic barrier
//     har round ke baad state reset karta.
// ============================================================================
#ifndef LFU_CACHE_LLD_UTILS_CONCURRENCYHELPERS_H
#define LFU_CACHE_LLD_UTILS_CONCURRENCYHELPERS_H

#include <condition_variable>
#include <mutex>

namespace lfu_cache_lld {

// ---- Sab threads ek saath start karne ke liye ------------------------------
class CyclicBarrier {
public:
    explicit CyclicBarrier(const int participantCount) : waiting_(participantCount) {
        (void)participantCount; // (member me already set; ye line sirf "unused" warning tang na kare)
    }

    void arriveAndWait() {
        std::unique_lock<std::mutex> lock(mutex_);
        if (--waiting_ == 0) {
            // Main hoon aakhri thread -> sabko jagao
            released_ = true;
            condition_.notify_all();
            return;
        }
        // Abhi baaki threads baaki hain -> so jao jab tak released_ na ho
        condition_.wait(lock, [this] { return released_; });
    }

private:
    int waiting_;                     // kitne abhi tak pahunchne baaki
    bool released_{false};            // sab pahunch gaye?
    std::mutex mutex_;
    std::condition_variable condition_;
};

// ---- Sab threads ke khatam hone ka wait karne ke liye ----------------------
class CountDownLatch {
public:
    explicit CountDownLatch(const int count) : count_(count) {}

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

} // namespace lfu_cache_lld

#endif // LFU_CACHE_LLD_UTILS_CONCURRENCYHELPERS_H

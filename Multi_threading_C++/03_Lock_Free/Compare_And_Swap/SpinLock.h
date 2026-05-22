#ifndef COMPARE_AND_SWAP_SPINLOCK_H
#define COMPARE_AND_SWAP_SPINLOCK_H

#include <atomic>
#include <bits/stdc++.h>

/**
 * Minimal spin lock using compare_exchange_weak on an atomic<bool>.
 * lock:  false -> true (acquire)
 * unlock: true -> false (release)
 */
class SpinLock {
public:
  void lock() {
    bool expected = false;
    while (!locked_.compare_exchange_weak(
        expected, true, std::memory_order_acquire, std::memory_order_relaxed)) {
      expected = false;
    }
  }

  void unlock() { locked_.store(false, std::memory_order_release); }

private:
  std::atomic<bool> locked_{false};
};

#endif // COMPARE_AND_SWAP_SPINLOCK_H

# 03 — Semaphore as Signaling (Connection Pool)

**Source:** `../03_semaphore_signaling.cpp`  
**Binary:** `../bin/03_semaphore_signaling`

---

## Pattern context

**Counting semaphore** = integer permits. `acquire` = wait for permit; `release` = signal permit available. Internally yahan bhi `condition_variable` use ho raha hai.

---

## Is file mein kya ho raha hai

- `CountingSemaphore pool(3)` — 3 connections
- 5 worker threads, har ek:
  1. `acquire()` — permit 0 ho to **block** (signal wait)
  2. Use connection 500ms
  3. `release()` — `notify_one` → ek waiter wake

---

## Class logic

```cpp
void acquire() {
    wait until permits_ > 0;
    --permits_;
}
void release() {
    ++permits_;
    cv_.notify_one();
}
```

---

## Advantages

| Fayda | Use case |
|-------|----------|
| Resource cap | DB pool, API rate slots |
| Sleep not spin | Blocked threads no CPU |
| Simple mental model | N slots = N permits |

**vs mutex alone:** Mutex ek cheez lock karta hai; semaphore **count** track karta hai.

---

## Run

```bash
./bin/03_semaphore_signaling
```

Pehle 3 workers turant "got connection"; baaki "waiting" until release.

---

## C++ note

C++20 has `std::counting_semaphore`; yahan C++17 custom class for learning.

Parent: [`../../semaphor.cpp`](../../semaphor.cpp)

---

## Interview

> "Semaphore signals availability of N identical resources; CV implements the wait queue."

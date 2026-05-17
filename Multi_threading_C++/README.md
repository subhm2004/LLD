# Multi-Threading C++ — Learning Module

Educational C++ concurrency snippets for **threads, locks, synchronization, and classic problems**.  
Har file ek alag topic cover karti hai — compile karke run karo, output se flow samjho.

> **Related LLD projects:** [LRU Cache (thread-safe)](../LRU_Cache_LLD/) · [Rate Limiter](../Rate_Limiter_LLD/) · [L10 Singleton thread-safe variants](../L10%20Singleton_Design_Pattern/)

---

## Prerequisites

- **C++17** compiler (`g++` / `clang++`)
- **pthread** flag on Linux/macOS

```bash
g++ -std=c++17 -pthread <file>.cpp -o <output>
./<output>
```

---

## Recommended Learning Order

| Step | File | Topic |
|------|------|-------|
| 1 | `lessson_1_join.cpp` | `std::thread`, `join`, `detach`, thread id |
| 2 | `race_condition_and_synchronization.cpp` | Race condition problem & fixes |
| 3 | `lesson_2_locks_and_mutex.cpp` | Mutex introduction |
| 4 | `types_of_locks.cpp` | `lock_guard`, `unique_lock`, `defer_lock`, `try_lock` |
| 5 | `lock_mechanism.cpp` | Lock patterns + `std::lock()` deadlock fix |
| 6 | `lesson_3.cpp` | `condition_variable` — ordered tasks |
| 7 | `producer_consumer.cpp` | Producer–consumer with CV |
| 8 | `semaphor.cpp` | Custom semaphore + connection pool |
| 9 | `deadlock_and_protection.cpp` | Deadlock demos & prevention |
| 10 | **`dining_philosophers.cpp`** | **Classic DP — 4 solutions** |
| 11 | `thread_pool.cpp` | Worker pool pattern |
| 12 | `DCLP.cpp` | Double-checked locking |
| 13 | `Thread_Safe_Injection.cpp` | Thread-safe dependency injection |
| 14 | `execution_time_of_code.cpp` | Measure execution time |
| 15 | [LRU_Cache_LLD](../LRU_Cache_LLD/) | Apply mutex in real LLD |

---

## File Index

| File | What you learn |
|------|----------------|
| `lessson_1_join.cpp` | Thread lifecycle, join vs detach, sleep, `get_id()` |
| `lesson_2_locks_and_mutex.cpp` | Basic mutex protecting shared counter |
| `lesson_3.cpp` | Turn-based execution with `condition_variable` |
| `race_condition_and_synchronization.cpp` | Data race → mutex fix |
| `lock_mechanism.cpp` | RAII locks, intentional deadlock, `std::lock` |
| `types_of_locks.cpp` | All lock types side-by-side comparison |
| `semaphor.cpp` | Counting/binary semaphore, DB pool analogy |
| `producer_consumer.cpp` | Bounded buffer with two condition variables |
| `deadlock_and_protection.cpp` | 2-mutex deadlock + multiple fixes |
| **`dining_philosophers.cpp`** | **5 philosophers, forks, 4 solution strategies** |
| `thread_pool.cpp` | Fixed worker threads + task queue |
| `DCLP.cpp` | Thread-safe lazy singleton pattern |
| `Thread_Safe_Injection.cpp` | Safe shared resource injection |
| `execution_time_of_code.cpp` | `chrono` benchmarking |

---

## Dining Philosophers (New)

**Compile & run:**

```bash
g++ -std=c++17 -pthread dining_philosophers.cpp -o dining_philosophers
./dining_philosophers
```

| Version | Approach | Deadlock? |
|---------|----------|-----------|
| 1 — Naive | Left fork → right fork | Yes (commented out in `main`) |
| 2 — Ordered forks | Always lock `min` then `max` index | No |
| 3 — Waiter | At most **N−1** philosophers at table | No |
| 4 — Try-lock | `try_lock` + random backoff | No (starvation rare) |

**Interview one-liner:**  
*"Circular wait on forks causes deadlock; break it with total ordering of mutexes, limit concurrent diners to N−1, or use try-lock with backoff."*

---

## Quick Compile Commands

```bash
cd Multi_threading_C++

g++ -std=c++17 -pthread lessson_1_join.cpp -o lesson1 && ./lesson1
g++ -std=c++17 -pthread producer_consumer.cpp -o pc && ./pc
g++ -std=c++17 -pthread thread_pool.cpp -o pool && ./pool
g++ -std=c++17 -pthread dining_philosophers.cpp -o dp && ./dp
g++ -std=c++17 -pthread deadlock_and_protection.cpp -o deadlock && ./deadlock
```

> **Note:** `deadlock_and_protection.cpp` me kuch demos intentionally hang kar sakte hain — file ke comments padho.

---

## Concept Map

```mermaid
flowchart TB
    subgraph basics [Basics]
        T[std::thread]
        J[join / detach]
    end

    subgraph sync [Synchronization]
        M[mutex]
        LG[lock_guard]
        UL[unique_lock]
        CV[condition_variable]
        SEM[semaphore]
    end

    subgraph problems [Classic Problems]
        PC[Producer Consumer]
        DP[Dining Philosophers]
        DL[Deadlock]
        TP[Thread Pool]
    end

    T --> M
    M --> LG
    M --> UL
    UL --> CV
    CV --> PC
    M --> DP
    M --> DL
    CV --> TP
```

---

## Interview Checklist

- [ ] Race condition vs data race vs deadlock
- [ ] `lock_guard` vs `unique_lock` — kab kaunsa?
- [ ] Why `condition_variable` needs `unique_lock`
- [ ] `notify_one` vs `notify_all`
- [ ] Producer–consumer bounded buffer
- [ ] Dining philosophers — 2+ solutions
- [ ] Thread pool — why reuse threads?
- [ ] DCLP — why double-checked locking is tricky
- [ ] LRU cache — why `get()` needs mutex (mutates order)

---

## Known Notes

- Filename typo: `lessson_1_join.cpp` (double **s**) — path change mat karo agar bookmarks ho.
- `bits/stdc++.h` kuch files me hai — macOS par explicit headers prefer karo (see `LRU_Cache_LLD`).
- Yeh folder **standalone LLD system nahi** hai — learning labs hain.

---

## Future Additions (optional)

- `atomic` + memory order demo
- `std::async` / `future` / `promise`
- `shared_mutex` reader–writer lock

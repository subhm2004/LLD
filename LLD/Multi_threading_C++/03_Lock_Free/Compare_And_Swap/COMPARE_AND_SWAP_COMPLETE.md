# Compare-And-Swap (CAS) — Complete Guide

> Aligned with [AlgoMaster CAS interview topic](https://algomaster.io/learn/concurrency-interview/compare-and-swap). Runnable demos: `01`–`06` in this folder.

---

## 1. What is Compare-And-Swap?

**CAS** ek **hardware primitive** hai (CPU instruction level), jo C++ mein `std::atomic` se expose hota hai:

```
CAS(addr, expected, new):
    if *addr == expected:
        *addr = new
        return SUCCESS
    else:
        expected = *addr   # actual value wapas
        return FAIL
```

**Teen steps ek saath atomic:**

1. Read current value
2. Compare with `expected`
3. Write `new` only if equal

Isliye **check-then-act race** break hota hai bina mutex ke (lock-free algorithms ka base).

---

## 2. Benefits of CAS

| Benefit | Detail |
|---------|--------|
| **No mutex blocking** | Threads spin/retry instead of sleeping in kernel |
| **Lock-free progress** | System-wide: koi thread block nahi karta doosre ko (at least one makes progress) |
| **Fine-grained updates** | Single counter / head pointer update without whole-structure lock |
| **Composable patterns** | Stacks, queues, counters, spin locks |

**Caveats:**

- **Contention** — zyada threads same atomic par fight → cache line bouncing
- **Complexity** — ABA, memory orders, safe memory reclamation
- **Not always faster** — short critical section + low contention → mutex often wins

---

## 3. How CAS Works (C++)

```cpp
std::atomic<int> x{100};
int expected = 100;
bool ok = x.compare_exchange_strong(expected, 80);
// ok == true  -> x is 80
// ok == false -> expected is updated to current x (e.g. 80)
```

| API | Use |
|-----|-----|
| `compare_exchange_strong` | Fail only on real mismatch |
| `compare_exchange_weak` | May fail spuriously — retry loop |
| `fetch_add`, `fetch_sub` | Often implemented with CAS internally |

**Demo:** `01_what_is_cas.cpp`, `03_compare_exchange_strong_weak.cpp`

---

## 4. CAS in Practice

### Lock-free counter

```cpp
counter.fetch_add(1, std::memory_order_relaxed);
```

**Demo:** `02_mutex_vs_atomic_counter.cpp`

### Spin lock

```cpp
while (!flag.compare_exchange_weak(expected, true, acquire, relaxed))
    expected = false;
```

**Demo:** `04_cas_spin_lock.cpp` + `SpinLock.h`

### Lock-free stack head (concept)

```cpp
// pop: CAS(head, A, A->next)  — ABA risk without version!
```

**Demo:** `05_aba_problem_and_version_stamp.cpp`

---

## 5. Example: Lock-Free Counter

| Approach | Code | Blocking? |
|----------|------|-----------|
| Mutex | `lock_guard` + `++counter` | Yes |
| Atomic | `fetch_add(1, relaxed)` | No (lock-free) |

4 threads × 250k increments — `02` timings compare karta hai (machine-dependent).

**Interview:** "Counter ke liye `std::atomic` + `fetch_add` usually enough; full lock-free queue tab jab latency/scale demand kare."

---

## 6. The ABA Problem

**Problem:** Value **A → B → A** wapas aa jaye, lekin beech mein structure change ho chuka ho.

Classic lock-free stack:

1. T1 reads `head = A`, next = B
2. T2 pops A, pops B, pushes A back → `head` phir A
3. T1 CAS `head: A → B` **success** — lekin A->next stale ho sakta hai

**Fixes:**

| Fix | Idea |
|-----|------|
| **Version / tagged pointer** | Har pop/push par version++ — CAS compares (ptr, version) |
| **Hazard pointers** | Safe reclamation — node delete tab jab koi read na kare |
| **Epoch-based reclamation** | Defer free until quiescent period |

**Demo:** `05_aba_problem_and_version_stamp.cpp`

---

## 7. Performance Considerations

| Factor | Effect |
|--------|--------|
| **Cache line contention** | Sab threads ek hi `atomic` — false sharing / bouncing |
| **Spin vs block** | Spin lock: CPU burn; mutex: kernel sleep when contended |
| **Memory order** | `relaxed` fastest; `seq_cst` default safest |
| **False sharing** | Pad atomics to separate cache lines in hot structures |

**Demo:** `06_memory_order_basics.cpp`

**Rule:** Pehle **correct mutex** solution; optimize to CAS jab profiling dikhaaye bottleneck.

---

## 8. Memory orders (cheat sheet)

| Order | Meaning |
|-------|---------|
| `relaxed` | Atomicity, no ordering with other atomics |
| `acquire` | Reads after this see writes before matching `release` |
| `release` | Writes before this visible after matching `acquire` |
| `acq_rel` | Both (read-modify-write ops) |
| `seq_cst` | Sequential consistency — default on `atomic` |

Producer-consumer flag pattern in `06`:

```cpp
payload.store(42, relaxed);
ready.store(true, release);
// consumer:
while (!ready.load(acquire)) {}
payload.load(relaxed);
```

---

## 9. Interview Q&A

**Q: CAS kya hai?**  
A: Atomic compare-and-swap — update only if memory equals expected; else retry with fresh expected.

**Q: Mutex se better?**  
A: Not always — CAS avoids blocking but complexity + contention cost badh sakti hai.

**Q: strong vs weak?**  
A: weak spurious fail — loops; strong one-shot updates.

**Q: ABA?**  
A: Same pointer value, different history — version stamp or hazard pointers.

**Q: `fetch_add` vs CAS loop?**  
A: `fetch_add` simpler for counters; CAS loop for arbitrary conditional updates (e.g. stack head).

---

## 10. Build & run all

```bash
./compile.sh
./bin/01_what_is_cas
./bin/02_mutex_vs_atomic_counter
./bin/03_compare_exchange_strong_weak
./bin/04_cas_spin_lock
./bin/05_aba_problem_and_version_stamp
./bin/06_memory_order_basics
```

---

## 11. Next steps

- [`../../02_Concurrency_Patterns/Producer_Consumer_Pattern/`](../../02_Concurrency_Patterns/Producer_Consumer_Pattern/) — mutex + CV (blocking queue)
- [`../../../Concurrent_HashMap_LLD`](../../../Concurrent_HashMap_LLD/) — lock striping LLD
- Read: [AlgoMaster CAS](https://algomaster.io/learn/concurrency-interview/compare-and-swap)

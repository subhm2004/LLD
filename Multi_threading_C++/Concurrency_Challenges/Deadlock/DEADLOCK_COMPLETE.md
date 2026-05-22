# Deadlock — Complete Guide (C++17)

> **Code:** `01`–`07` in this folder — narrative in [`README.md`](./README.md)

---

## Table of contents

1. [Deadlock kya hai](#1-deadlock-kya-hai)
2. [Coffman's 4 conditions](#2-coffmans-4-conditions)
3. [Classic 2-mutex example](#3-classic-2-mutex-example)
4. [Prevention strategies](#4-prevention-strategies)
5. [Detection & recovery](#5-detection--recovery)
6. [Har demo explained](#6-har-demo-explained)
7. [Deadlock vs livelock vs starvation](#7-deadlock-vs-livelock-vs-starvation)
8. [Interview Q&A](#8-interview-qa)
9. [Tools & golden rules](#9-tools--golden-rules)

---

## 1. Deadlock kya hai

**Deadlock** = 2+ threads **permanently blocked**, har thread kisi doosri thread ke paas lock/resource ka wait karti hai — **zero progress**.

**Real life:** Narrow road — do gaadi aamne saamne, dono ek doosre ke hatne ka wait — koi move nahi.

```mermaid
flowchart LR
    T1[Thread 1 holds A] -->|waits B| T2[Thread 2 holds B]
    T2 -->|waits A| T1
```

**Symptoms:** CPU low (threads sleeping on mutex), program hang, logs stop mid-way.

---

## 2. Coffman's 4 conditions

Deadlock **tabhi** possible jab **charo** true hon — koi **ek** todo → deadlock impossible.

| # | Condition | Matlab | Example |
|---|-----------|--------|---------|
| 1 | **Mutual exclusion** | Resource ek waqt ek hi thread | `mutex` |
| 2 | **Hold and wait** | Lock hold + doosra wait | Hold A, wait B |
| 3 | **No preemption** | Forcefully lock nahi cheena | No OS steal |
| 4 | **Circular wait** | Cycle in wait graph | T1→B, T2→A |

**Demo:** `01_coffman_four_conditions.cpp`

---

## 3. Classic 2-mutex example

```text
Thread 1: lock(A) → lock(B)
Thread 2: lock(B) → lock(A)   ← opposite order → circular wait
```

**Timeline:**

```text
t0: T1 has A, T2 has B
t1: T1 waits B (held by T2)
t1: T2 waits A (held by T1)
→ forever
```

**Demo:** `02_opposite_order_deadlock.cpp` — `timed_mutex` + `try_lock_for` se hang avoid, message print.

**Real hang:** `07_optional_infinite_deadlock.cpp` — uncomment manually only.

---

## 4. Prevention strategies

### Fix 1 — Same lock order (break circular wait)

```cpp
// Both threads: always A then B
mtxA.lock();
mtxB.lock();
```

**Team rule:** Global order (alphabetical mutex names).

**Demo:** `03_fix_same_lock_order.cpp`

---

### Fix 2 — `std::lock` (break hold-and-wait)

```cpp
unique_lock<mutex> la(mtxA, defer_lock);
unique_lock<mutex> lb(mtxB, defer_lock);
lock(la, lb);  // both or neither
```

Internally: try both, if fail release all, retry — **no partial hold**.

**Demo:** `04_fix_std_lock.cpp`

---

### Fix 3 — `scoped_lock` C++17 (recommended)

```cpp
scoped_lock lk(mtxA, mtxB);
// work
```

One line, RAII unlock, same as `std::lock` inside.

**Demo:** `05_fix_scoped_lock.cpp`

---

### Fix 4 — `try_lock` + backoff

```cpp
if (mtxA.try_lock()) {
    if (mtxB.try_lock()) { /* work */ unlock both; }
    else mtxA.unlock();  // CRITICAL — no partial hold
}
sleep/backoff;
```

**Demo:** `06_try_lock_livelock_starvation.cpp`

---

## 5. Detection & recovery

| Approach | How |
|----------|-----|
| **Prevention** | Design — fixes above |
| **Avoidance** | Banker's algorithm (theory) |
| **Detection** | Wait-for graph cycle |
| **Recovery** | Kill thread, rollback, try_lock timeout |

**This repo:** `02` timed try = soft detection; `07` shows true block.

**Tools:** `g++ -fsanitize=thread`, gdb `info threads`, profilers.

---

## 6. Har demo explained

| File | Kya hota hai | Kaunsi condition todti |
|------|--------------|------------------------|
| 01 | Print 4 Coffman conditions | Education |
| 02 | T1 A→B, T2 B→A, timeout on 2nd lock | Shows circular wait |
| 03 | Both A→B | Circular wait ❌ |
| 04 | `std::lock` both | Hold-and-wait ❌ |
| 05 | `scoped_lock` | Same as 04, clean API |
| 06 | try_lock retry + DL vs LL vs starvation | Non-blocking attempt |
| 07 | Commented infinite deadlock | Manual feel |

---

## 7. Deadlock vs livelock vs starvation

| | Deadlock | Livelock | Starvation |
|--|----------|----------|------------|
| **Progress** | None | None useful | Unfair slow |
| **CPU** | Low (blocked) | High (retry loop) | Varies |
| **Threads** | Blocked on mutex | Running but yielding | One always loses |
| **Example** | A↔B wait | Both try_lock, both release, repeat | Low priority never runs |
| **Fix** | Lock ordering | Random backoff | Fair queue |

---

## 8. Interview Q&A

**Q: Deadlock ki 4 conditions?**  
Mutual exclusion, hold-and-wait, no preemption, circular wait.

**Q: Sabse easy fix?**  
Same global lock order on all mutexes.

**Q: Best C++17 fix for 2 mutex?**  
`scoped_lock(m1, m2)`.

**Q: `std::lock` vs `scoped_lock`?**  
`scoped_lock` = RAII wrapper; `std::lock` on `unique_lock` with `defer_lock`.

**Q: try_lock partial hold?**  
If second fails, release first — else hold-and-wait returns.

**Q: Dining philosophers?**  
Classic deadlock — fix: order forks, N-1 philosophers, try_lock.

**Q: Deadlock vs race?**  
Deadlock = hang, no forward; race = wrong data, may continue.

---

## 9. Tools & golden rules

### Golden rules

1. **Minimize** lock count and hold time  
2. **Same order** for all threads  
3. **2+ mutex** → `scoped_lock`  
4. **No I/O/sleep** under lock  
5. **try_lock** → release on failure + backoff  
6. Test with **ThreadSanitizer**

### Compile with TSan

```bash
g++ -std=c++17 -pthread -fsanitize=thread -g file.cpp -o app
```

### Comparison table

| Fix | C++ | Deadlock safe | Ease |
|-----|-----|---------------|------|
| Same order | 11+ | ✅ | Easy |
| `std::lock` | 11+ | ✅ | Medium |
| `scoped_lock` | 17 | ✅ | Easiest |
| try_lock+retry | 11+ | Mostly | Harder |

---

**Original monolith:** [`../../deadlock_and_protection.cpp`](../../deadlock_and_protection.cpp)

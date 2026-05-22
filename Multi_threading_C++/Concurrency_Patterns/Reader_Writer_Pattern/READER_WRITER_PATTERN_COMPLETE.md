# Reader-Writer Pattern — Complete Guide (C++17)

> **Pehle padho:** [`README.md`](./README.md)  
> **Custom lock:** [`ReadWriteLock.h`](./ReadWriteLock.h)  
> **Run:** `./compile.sh` → `./bin/01_std_shared_mutex_basics`

---

## Table of contents

1. [Problem kya solve karta hai](#1-problem-kya-solve-karta-hai)
2. [Readers vs writers — rules](#2-readers-vs-writers--rules)
3. [mutex vs reader-writer lock](#3-mutex-vs-reader-writer-lock)
4. [std::shared_mutex — C++17 standard](#4-stdshared_mutex--c17-standard)
5. [ReadWriteLock.h — custom implementation](#5-readwritelockh--custom-implementation)
6. [Reader vs writer preference](#6-reader-vs-writer-preference)
7. [Starvation](#7-starvation)
8. [Har demo — deep dive](#8-har-demo--deep-dive)
9. [Thread-safe cache pattern](#9-thread-safe-cache-pattern)
10. [Lock upgrade problem](#10-lock-upgrade-problem)
11. [When to use / not use](#11-when-to-use--not-use)
12. [Common bugs](#12-common-bugs)
13. [Interview Q&A](#13-interview-qa)
14. [Pattern relations](#14-pattern-relations)

---

## 1. Problem kya solve karta hai

Shared data (config, cache, document) par:

- **Bahut threads READ** karti hain — data change nahi hota read ke dauran
- **Kam threads WRITE** karti hain — data change hota hai — **exclusive** access chahiye

**Naive:** Har read par `mutex` → reads **serial** → CPU waste.

**Reader-Writer lock:** Multiple reads **parallel**, writes **exclusive**.

---

## 2. Readers vs writers — rules

### Compatibility matrix

| Current holder | New reader | New writer |
|----------------|------------|------------|
| Nothing | ✅ enter | ✅ enter |
| Readers only | ✅ enter | ❌ wait |
| Writer active | ❌ wait | ❌ wait |

### Invariants

```text
writers_active ≤ 1
readers_active ≥ 0
if writer_active → readers_active must be 0
```

---

## 3. mutex vs reader-writer lock

| | `mutex` | `shared_mutex` |
|--|---------|----------------|
| Read + Read | Serialized ❌ | Parallel ✅ |
| Read + Write | Exclusive | Exclusive |
| Write + Write | Exclusive | Exclusive |
| Complexity | Low | Medium |
| Best when | Write heavy / simple | **Read heavy** |

**Rule of thumb:** Read:Write ratio > ~10:1 → consider RW lock.

---

## 4. std::shared_mutex — C++17 standard

### Types

```cpp
#include <shared_mutex>

shared_mutex m;

shared_lock<shared_mutex> rl(m);   // multiple readers
unique_lock<shared_mutex> wl(m);   // one writer
```

### RAII meaning

| Lock | Acquire | Release |
|------|---------|---------|
| `shared_lock` | shared (read) | destructor unlocks read |
| `unique_lock` | exclusive (write) | destructor unlocks write |

### Demo 01 flow

- 2 readers: `shared_lock`, print `shared_value`, sleep 80ms — **overlap possible**
- 1 writer: `unique_lock`, `++shared_value` — **no reader inside**

**Production:** Prefer `shared_mutex` unless custom fairness policy chahiye.

---

## 5. ReadWriteLock.h — custom implementation

### Reader-preference (`ReadWriteLock`)

**`lock_shared()` (reader):**

```text
wait until writer_active_ == false
readers_++
```

**`unlock_shared()`:**

```text
readers_--
if readers_ == 0 → notify_all (waiting writers)
```

**`lock()` (writer):**

```text
wait until readers_ == 0 AND NOT writer_active_
writer_active_ = true
```

**`unlock()`:**

```text
writer_active_ = false
notify_all
```

Uses **Signaling** (`condition_variable`) — see [`../Signaling_Pattern/SIGNALING_PATTERN_COMPLETE.md`](../Signaling_Pattern/SIGNALING_PATTERN_COMPLETE.md).

### Writer-preference (`WriterPreferenceRWLock`)

Extra flag `writer_waiting_`:

- Writer `lock()` pehle `writer_waiting_=true`
- New readers wait if `writer_waiting_` — writer ko turn jaldi

Demo 06 compares behavior.

---

## 6. Reader vs writer preference

| Policy | Kaun favor | Nuksan |
|--------|------------|--------|
| **Reader preference** | Read throughput | Writer starvation (constant readers) |
| **Writer preference** | Fresh writes land | Reader starvation (constant writers) |
| **Fair (FIFO)** | Balanced | Complex queue |

`std::shared_mutex` fairness — **implementation defined** (libc++/libstdc++ differ). Interview mein bolo: "depends on STL/OS".

---

## 7. Starvation

### Writer starvation (reader-pref)

```text
Readers keep arriving → readers_ never 0 → writer waits forever
```

**Fix ideas:** max readers spin, writer queue priority, writer-pref lock.

### Reader starvation (writer-pref)

Constant writers → readers rarely enter.

**Fix:** fair queue, read timeout.

---

## 8. Har demo — deep dive

### 01 — `01_std_shared_mutex_basics.cpp`

| Actor | Lock | Action |
|-------|------|--------|
| reader 1,2 | shared | Read value 3× |
| writer | unique | Increment 2× |

**Final value:** 2.  
**Lesson:** Standard API — daily driver.

---

### 02 — `02_custom_read_write_lock.cpp`

Same scenario with `ReadWriteLock` — `lock_shared` / `lock` / `unlock`.

**Why learn custom:** Interview "implement RW lock" whiteboard.

**Note:** Variable `shared_data` — `data` name avoid (`std::data` conflict with `using namespace std`).

---

### 03 — `03_many_readers_parallel.cpp`

- 5 readers, **no writer**
- `atomic concurrent_reads`, track `max_concurrent_reads`
- **Proof:** max ≥ 2 → parallel shared locks work

---

### 04 — `04_writer_exclusive_access.cpp`

- Reader: snapshot `counter`, sleep, verify unchanged
- Writer: 200ms exclusive `counter += 10`
- **Consistency:** Read lock = stable view for read duration

Agar write bina lock during read → `ERROR: value changed` print (demo check).

---

### 05 — `05_thread_safe_cache.cpp`

`ThreadSafeCache`:

| Method | Lock | Why |
|--------|------|-----|
| `get` | `shared_lock` | Parallel cache hits |
| `put` | `unique_lock` | Map insert/update structural |
| `size` | `shared_lock` | Read metadata |

3 readers hammer `get("user:1")`, main `put` update — readers eventually see new value.

**Pitfall:** Return pointer to internal data — copy return (`optional<string>`) safe.

---

### 06 — `06_reader_vs_writer_preference.cpp`

Stress function template on both lock types.

**Observe:** Writer-pref — writer log sooner when readers constantly running.

---

## 9. Thread-safe cache pattern

**Read-mostly workload** = RW lock sweet spot.

```text
99% get()  → shared_lock  → scale readers
1% put()   → unique_lock  → rare exclusive
```

Alternatives:

| Approach | When |
|----------|------|
| `mutex` every op | Simple, low read ratio |
| `shared_mutex` | High read ratio |
| Immutable + atomic swap | Config reload rare |
| RCU (advanced) | Linux kernel style |

---

## 10. Lock upgrade problem

**Want:** Read lock → discover need write → upgrade to write lock.

**Problem:** `shared_mutex` **no atomic upgrade**.  
**Wrong:** Hold shared, acquire unique → **deadlock** (writer waits self).

**Pattern:**

```cpp
// 1. Read
{
  shared_lock lk(m);
  if (!need_write) return read_data();
}
// 2. Release read, then write
{
  unique_lock lk(m);
  update();
}
```

**Gap between 1 and 2:** Doosra thread change kar sakta hai — design for that (double-check, version number).

---

## 11. When to use / not use

### Use ✅

- Config server (reads 1000/s, writes 1/min)
- In-memory cache
- Graph/document many readers
- Routing table lookups

### Avoid ❌

- Write ratio high (>30%)
- Critical section 2 instructions — lock overhead dominates
- Need strict FIFO fairness — roll custom
- Same thread recursive read — not supported in basic RW

---

## 12. Common bugs

| Bug | Fix |
|-----|-----|
| Write with `shared_lock` | Use `unique_lock` |
| Read without lock | Always `shared_lock` |
| Read return `&internal` | Copy out |
| Writer inside read lock | Release read first |
| `using namespace std` + var `data` | Rename — ambiguous with `std::data` |

---

## 13. Interview Q&A

**Q: RW lock vs mutex?**  
Parallel reads; mutex serializes everything.

**Q: How many readers at once?**  
Unlimited in theory; practical limits by impl/OS.

**Q: Writer active — new reader?**  
Blocked until writer unlocks.

**Q: Implement RW lock?**  
`readers` count, `writer` flag, 1–2 CVs, mutex.

**Q: shared_timed_mutex?**  
C++14/17 — shared + timeout variants.

**Q: Database "read committed"?**  
Conceptually related — readers see snapshot; different isolation levels stricter.

---

## 14. Pattern relations

```text
Signaling (CV wait/notify)
    └── ReadWriteLock custom (wait readers==0)
    └── Producer-Consumer (different problem — buffer)
    └── Thread Pool (task queue — not RW)
```

- [`../Signaling_Pattern/SIGNALING_PATTERN_COMPLETE.md`](../Signaling_Pattern/SIGNALING_PATTERN_COMPLETE.md)
- [`../../types_of_locks.cpp`](../../types_of_locks.cpp)

---

## Quick revision cheat sheet

```text
READ  → shared_lock(shared_mutex)  OR lock_shared()
WRITE → unique_lock(shared_mutex)  OR lock() / unlock()
Many readers OK | One writer only
Reader-pref vs Writer-pref = starvation tradeoff
Production: prefer std::shared_mutex unless need custom policy
```

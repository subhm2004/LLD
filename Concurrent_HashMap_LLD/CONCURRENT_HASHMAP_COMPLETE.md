# Concurrent HashMap — Complete Design (C++17)

> **Run:** `./concurrent_hashmap_app`

---

## 1. Problem statement

Implement `ConcurrentHashMap` — multiple threads call `put` / `get` / `remove` without:

- Corrupting buckets
- Lost updates (unless defined — we use last-writer-wins)
- Data races on `unordered_map` internals

**Not in v1:** Distributed sharding (Redis cluster), persistent map, weakly consistent iteration.

---

## 2. Data structure fundamentals

```text
key --hash--> stripe index --mutex--> local unordered_map
```

| Piece | Role |
|-------|------|
| `hash(key)` | Map key → stripe id |
| `stripes_` | Count of independent segments |
| `buckets_[i]` | `unordered_map` for stripe i |
| `locks_[i]` | `mutex` guarding bucket i |

**Why not one big `unordered_map`?** STL containers are not thread-safe for concurrent write/read without external sync.

---

## 3. Concurrency challenges

| Challenge | Risk | Mitigation |
|-----------|------|------------|
| Concurrent `put` same bucket | Race on bucket list | Per-stripe mutex |
| `get` during `put` | Torn read / UB | Lock stripe before read |
| `size()` across stripes | Snapshot approximate | Lock each stripe sequentially |
| Hot key | All threads same stripe → serial | More stripes, or per-key lock (advanced) |
| Rehash during resize | Iterator invalidation | Global lock on resize (extension) |

---

## 4. Approach 1 — Coarse-grained locking

```cpp
mutex mtx_;
unordered_map<string, string> store_;

void put(k, v) {
    lock_guard<mutex> lock(mtx_);
    store_[k] = v;
}
```

| Pros | Cons |
|------|------|
| Trivially correct | Every op serializes |
| Easy interview start | Poor scalability |

**File:** `CoarseGrainedHashMap.h`

---

## 5. Approach 2 — Lock striping (fine-grained)

```cpp
size_t idx = hash(key) % num_stripes;
lock_guard<mutex> lock(locks_[idx]);
buckets_[idx][key] = value;
```

| Pros | Cons |
|------|------|
| Different keys → parallel | Same stripe still serial |
| Good default for interviews | `size()` locks all stripes |
| Tunable `num_stripes` | More memory (N mutexes) |

**File:** `StripedHashMap.h` — **default** `ConcurrentHashMap` alias.

### Stripe count tuning

- Too few → contention remains
- Too many → memory + lock overhead, sparse buckets
- Common: 2×–16× CPU cores, power of 2

---

## 6. Approach 3 — CAS-based (concept)

Lock-free bucket head with `compare_exchange` on linked nodes.

| Pros | Cons |
|------|------|
| Max throughput | Complex, ABA, memory reclamation |
| No mutex blocking | Hard to implement in interview timebox |

**This repo:** theory only — implement as extension.

---

## 7. Solution comparison

| Criteria | Coarse | Striping | CAS |
|----------|--------|----------|-----|
| Correctness | Easy | Easy | Hard |
| Read/write parallel | No | Partial | High |
| Memory | Low | Medium | Medium |
| Interview time | 5 min | 15 min | 45+ min |

---

## 8. Implementation walkthrough

### `put`

```text
idx = stripeIndex(key)
lock locks_[idx]
buckets_[idx][key] = value
```

### `get`

```text
idx = stripeIndex(key)
lock locks_[idx]
find in buckets_[idx]
return optional value
```

### `remove`

Same stripe lock, `erase(key)`.

### `size`

Sum `buckets_[i].size()` with each `locks_[i]` held — **point-in-time** estimate under concurrency.

---

## 9. `main.cpp` demos

| Demo | Shows |
|------|-------|
| Basic | put/get/miss |
| 4×100 puts | 400 keys parallel stripes |
| Hot key | 8 threads same key — contention |
| Benchmark | Coarse vs striped timing |
| Readers | Concurrent `get` |

---

## 10. Interview Q&A

**Q: Why striping vs one RW lock on whole map?**  
Striping allows unrelated keys to proceed in parallel; single RW lock serializes all readers too if using mutex.

**Q: Two keys same stripe?**  
They share one mutex — potential false sharing of lock.

**Q: `hash % N` distribution?**  
Assume uniform hash; bad hash function → imbalance.

**Q: Java `ConcurrentHashMap`?**  
Segmented locks + CAS + sizeCtl — industrial version of striping + optimizations.

**Q: Iterator thread-safe?**  
Not in this LLD — snapshot under global lock required.

---

## 11. Extensions

- Per-stripe `shared_mutex` for read-heavy
- `std::shared_lock` on `get` when key stable
- Dynamic resize with `std::scoped_lock` on all stripes
- `atomic<size_t>` approximate size

---

## 12. Related study

- [`Thread_Safe_Cache_with_TTL_LLD`](../Thread_Safe_Cache_with_TTL_LLD/)
- [`Multi_threading_C++/Concurrency_Patterns/Reader_Writer_Pattern/`](../Multi_threading_C++/Concurrency_Patterns/Reader_Writer_Pattern/)

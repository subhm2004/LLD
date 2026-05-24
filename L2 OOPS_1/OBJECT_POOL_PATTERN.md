# Object Pool Pattern — Memory & Allocation Optimization

> **Code:** [`C++ Code/19_Object_Pool_Pattern.cpp`](./C%20%2B%2B%20Code/19_Object_Pool_Pattern.cpp)  
> **Related (threads):** [Thread Pool](../Multi_threading_C++/02_Concurrency_Patterns/Thread_Pool_Pattern/) — recycles **threads**, not arbitrary objects

---

## Table of Contents

1. [Problem](#1-problem)
2. [Object pool idea](#2-object-pool-idea)
3. [Repo implementation walkthrough](#3-repo-implementation-walkthrough)
4. [Acquire / release contract](#4-acquire--release-contract)
5. [Thread safety](#5-thread-safety)
6. [vs other patterns](#6-vs-other-patterns)
7. [Interview Q&A](#7-interview-qa)

---

## 1. Problem

Har request par:

```cpp
auto c = make_unique<Connection>();
// use ...
// destroy
```

| Cost | Impact |
|------|--------|
| `new` / `delete` | Heap churn, allocator lock |
| Constructor | Heavy setup (socket, auth) |
| Fragmentation | Many short-lived objects |

Games (bullets), DB pools, network connections — **same object type**, **high frequency**.

---

## 2. Object pool idea

```
Startup:  pre-create N objects (pool full)
Acquire:  take idle object from pool (or grow)
Use:      reset state, do work
Release:  return to pool — DO NOT delete
Shutdown: pool destructor frees all
```

| Benefit | Trade-off |
|---------|-----------|
| Reuse memory | Must `reset()` state correctly |
| Predictable max memory | Pool size tuning |
| Faster hot path | Bookkeeping (`queue` of free ptrs) |

---

## 3. Repo implementation walkthrough

```cpp
class ConnectionPool {
    vector<unique_ptr<Connection>> storage;  // owns lifetime
    queue<Connection*> available;            // free list
public:
    Connection* acquire();
    void release(Connection* c);
};
```

| Piece | Role |
|-------|------|
| `storage` | **Owns** all `Connection` objects for program lifetime |
| `available` | Idle pointers ready to reuse |
| `acquire()` | Pop free, or allocate overflow |
| `release()` | `reset()` + push back — **no delete** |

---

## 4. Acquire / release contract

**Caller rules:**

1. `release()` exactly once per `acquire()` (same pointer).
2. Don't use pointer after `release()`.
3. Don't `delete` pooled object yourself.

**Pool rules:**

1. `reset()` clears **transient** state (flags, buffers), not identity.
2. Growing pool — cap max size in production to avoid unbounded growth.

---

## 5. Thread safety

Repo demo is **single-threaded**.

Production:

```cpp
mutex m;
Connection* acquire() {
    lock_guard<mutex> g(m);
    // pop from queue
}
```

Or **thread-local pools** to reduce contention.

---

## 6. vs other patterns

| Pattern | Reuses |
|---------|--------|
| **Object pool** | **Objects** (Connection, Enemy) |
| **Flyweight** | **Shared intrinsic state** (glyphs) — many clients share one |
| **Prototype** | **Copy** from template object |
| **Arena / bump allocator** | Memory blocks, not typed objects |
| **Thread pool** | Worker threads |

---

## 7. Interview Q&A

<details>
<summary><strong>Object pool kyun?</strong></summary>

Avoid repeated allocation + expensive ctor on hot path; bound memory.

</details>

<details>
<summary><strong>Pool vs singleton?</strong></summary>

Pool = **many** reusable instances. Singleton = **one** global instance.

</details>

<details>
<summary><strong>Danger?</strong></summary>

Use-after-release, forgotten `reset()`, double `release()`, thread races.

</details>

---

## Cheat sheet

```
POOL          pre-allocate → acquire → use → release (no delete)
OWNERSHIP     pool owns (vector<unique_ptr>)
RESET         mandatory on release
THREADS       mutex or per-thread pool
```

---

⬅️ [Covariant (L3)](../L3%20OOPS_2/COVARIANT_RETURN_TYPES.md) · [RAII](./OOPS_ADVANCED_CPP.md#4-raii)

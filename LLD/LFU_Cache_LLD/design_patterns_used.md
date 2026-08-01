# Design Patterns Used — LFU Cache LLD

Ye ek **thread-safe, average O(1) LFU cache** hai. Ise clean aur extensible rakhne ke liye
ek behavioural pattern (Strategy via interface), ek structural pattern (Decorator for
thread-safety), aur ek Facade (service layer) use hue hain. Saath me kuch important
concurrency + data-structure techniques hain jo ise "interview-grade" banati hain.

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                            PATTERN OVERVIEW                                    │
├──────────────────┬─────────────────────────────────┬─────────────────────────┤
│  Pattern         │  Kahan (class)                  │  Kis liye                │
├──────────────────┼─────────────────────────────────┼─────────────────────────┤
│  Strategy        │  ICache<K,V> interface          │  eviction policy pluggable│
│  Decorator ⭐    │  ThreadSafeLFUCache             │  thread-safety wrapper   │
│  Facade          │  CacheService                   │  cache + stats ek jagah  │
│  (technique)     │  LFUCacheCore                   │  O(1) buckets + minFreq  │
│  (technique)     │  CacheStatistics                │  atomic lock-free counters│
└──────────────────┴─────────────────────────────────┴─────────────────────────┘
```

---

## 1. Strategy Pattern — `ICache` interface

### 📌 Concept
Cache ke operations (get/put/contains/...) ka ek generic **contract** define karo, aur
"kaise" ko concrete classes pe chhod do. Isse eviction policy plug-and-play ban jaati hai.

### PATTERN KE ROLES
- **Strategy interface** → [`ICache<Key, Value>`](./cache/ICache.h)
- **Concrete strategies** → [`LFUCacheCore`](./cache/LFUCacheCore.h), [`ThreadSafeLFUCache`](./cache/ThreadSafeLFUCache.h) (aur sibling folder ka `LRUCacheCore`)

### ⭐ KYUN?
> Client sirf `ICache<K,V>*` pakadta hai — usse pata nahi andar LFU hai ya LRU. Isi repo me
> `LFU_Cache_LLD` aur `LRU_Cache_LLD` **same `ICache` interface** implement karte hain, sirf
> algorithm alag. Policy swap karni ho to concrete class badlo — client code untouched.
> **Open/Closed Principle.**

---

## 2. Decorator Pattern — `ThreadSafeLFUCache` ⭐

### 📌 Concept
Ek object ko badle bina uske upar nayi zimmedari (behavior) "wrap" kar do. Wrapper wahi
interface implement karta hai, to bahar se same lagta hai — bas ek super-power extra.

### PATTERN KE ROLES
- **Component (interface)** → `ICache`
- **Concrete component** → `LFUCacheCore` (pure algorithm, locks se anjaan)
- **Decorator** → [`ThreadSafeLFUCache`](./cache/ThreadSafeLFUCache.h) — har call ko mutex ke peeche wrap karta hai

### ⭐ KYUN? (Why Decorator over baking locks into core)
> `LFUCacheCore` sirf LFU algorithm janta hai (Single Responsibility). Locking ek **alag
> concern** hai jise decorator add karta hai. Fayda: single-thread me core seedha use karo
> (bina lock overhead), multi-thread me wrapper. Ek hi algorithm dono jagah reuse — koi duplication.

### ⚠ Sabse important insight: `get()` bhi lock leta hai
> Aam soch: "read safe hoga". Par LFU me **get() cache ko badalta hai** — frequency badhata
> hai, node ko doosre bucket me shift karta. Yaani get bhi ek WRITE hai! Isi liye `shared_mutex`
> (read-parallelism) yahan kaam nahi karta — har get bhi exclusive lock leta hai.

---

## 3. Facade Pattern — `CacheService`

### 📌 Concept
Multiple cheezon (cache + statistics) ko ek patle interface ke peeche jodo.

### Implementation
[`CacheService`](./core/CacheService.h) har operation ko cache pe forward karta hai **aur**
saath me statistics (hit/miss/eviction) khud update kar deta hai:
```cpp
service.get(key);  // andar: cache.get() + (hit? recordHit : recordMiss)
service.put(k, v); // andar: cache.put() + recordPut + (evicted? recordEviction)
```

### ⭐ KYUN?
> Metrics ek "cross-cutting" concern hai — usse cache algorithm me ghusaane ke bajaye service
> layer pe wrap kiya. Cache core apne kaam pe focus rehta, aur wahi `CacheStatistics` kisi
> bhi cache (LFU/LRU) ke saath reuse ho jaati.

---

## 4. Technique: O(1) LFU — buckets + `minFreq` pointer ⭐

Ye GoF pattern nahi, par is folder ka **asli dil** hai (LFUCacheCore.h). 3 data structures:

| Structure | Kaam |
|-----------|------|
| `keyMap_` : key → Node (`unique_ptr`) | O(1) lookup + node ka OWNER |
| `freqBuckets_` : freq → `list<Node*>` | ek frequency ke saare nodes (raw ptrs) |
| `minFreq_` : int | evict pointer — sabse choti frequency |

**Eviction:** `minFreq_` wale bucket ke `back()` se nikaalo = lowest frequency, aur usme se
LRU (sabse purana). **Tie-break LRU** apne aap ho jaata hai kyunki naye nodes bucket ke `front`
me jaate hain, to `back` hamesha sabse purana rehta hai.

**Har node apna `bucketIterator` store karta hai** → list se O(1) erase (poori list scan nahi).

> **⭐ Ownership note:** node ka maalik `keyMap_` (unique_ptr) hai; buckets sirf raw pointer
> rakhte hain. Isi liye remove/evict me **order** critical: pehle bucket se nikaalo, PHIR
> keyMap se erase (warna raw pointer dangling). Ye memory-safety ka classic point hai.

---

## 5. Technique: Lock-free atomic statistics

[`CacheStatistics`](./stats/CacheStatistics.h) me counters `std::atomic<long long>` hain.
Multiple threads bina mutex ke `fetch_add` se safely count karte hain. `memory_order_relaxed`
use kiya kyunki ye **sirf counters** hain (kisi synchronization/ordering ke liye nahi) — sabse
sasta atomic mode, atomicity deta hai bina extra overhead ke.

---

## Object Relationships (UML samajhne ke liye)

```
CacheService     ◆──  ThreadSafeLFUCache   (Composition: service owns cache)
CacheService     ◆──  CacheStatistics      (Composition)
ThreadSafeLFUCache ◆── LFUCacheCore         (Composition: decorator wraps core)
ThreadSafeLFUCache ─▷  ICache               (implements)
LFUCacheCore     ─▷   ICache               (implements)
LFUCacheCore     ◆──  LFUNode (keyMap)      (Composition: unique_ptr OWNER)
freqBuckets_     ··▶  LFUNode               (Association: raw pointers, own nahi karta)
```
- `◆──` Composition (owns) · `─▷` Inheritance (implements) · `··▶` weak Association (raw ptr)

---

## Requirement → Pattern/Technique quick map

| Requirement | Kaise pura hua |
|-------------|----------------|
| Positive capacity | `CacheConfig` constructor validation (fail-fast) |
| get hit → value + freq++ | `LFUCacheCore::get` + `increaseFrequency` |
| get miss → nullopt | `std::optional` return |
| put evict over capacity | `evictLeastFrequentlyUsed` (minFreq bucket back) |
| min-freq eviction, LRU tie | buckets `front`/`back` ordering |
| statistics | `CacheStatistics` atomic counters |
| thread safety | **Decorator** `ThreadSafeLFUCache` (mutex) |
| average O(1) | keyMap + freqBuckets + minFreq + stored iterator |
| no `bits/stdc++.h` (NFR4) | ✅ saare files targeted includes pe shift kar diye |

> **Cleanup note:** pehle har file me `#include <bits/stdc++.h>` tha (jo NFR4 "no bits/stdc++.h"
> ko violate karta aur non-portable/GCC-only hai). Ise **hata diya** — har file ab sirf apne
> zaroori standard headers include karti hai.

---

## "Ye pattern kyun NAHI use kiya" (interview me pooche jaate hain)

- **`shared_mutex` (reader-writer lock) get() ke liye?** Nahi chalega — LFU me get() frequency
  badal ke cache ko **mutate** karta hai, to wo shared (read) lock nahi le sakta. Sabhi ops exclusive.
- **Factory for policy selection?** `EvictionPolicyType` enum seed hai, par abhi ek hi policy
  (LFU) hai is folder me. Multiple policies ek jagah se chahiye ho tab factory add karenge — abhi YAGNI.
- **Singleton cache?** Cache ek dependency hai jo inject hoti hai (`CacheService` me). Singleton
  global state deta jo testing mushkil karta — zaroorat na ho to mat lagao.
- **Sharded locks abhi kyun nahi?** Ek mutex = simple aur **sahi** (correctness first). High
  concurrency pe `hash(key) % N` sharding se scale hota — wo ek natural extension hai, abhi over-engineering.
```

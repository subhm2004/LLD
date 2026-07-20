# Design Patterns Used — LRU Cache LLD

Ye ek **thread-safe, average O(1) LRU cache** hai. Ise clean aur extensible rakhne ke liye
ek behavioural pattern (Strategy via interface), ek structural pattern (Decorator for
thread-safety), aur ek Facade (service layer) use hue hain. Saath me `std::list::splice`
aur atomic counters jaisi techniques ise "interview-grade" banati hain.

> 📌 Policy comparison (LRU vs LFU — kab kaunsa) ke liye alag detailed doc hai:
> [`WHY_LRU_OVER_LFU.md`](./WHY_LRU_OVER_LFU.md). Ye file sirf **design patterns** pe focus karti hai.

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                            PATTERN OVERVIEW                                    │
├──────────────────┬─────────────────────────────────┬─────────────────────────┤
│  Pattern         │  Kahan (class)                  │  Kis liye                │
├──────────────────┼─────────────────────────────────┼─────────────────────────┤
│  Strategy        │  ICache<K,V> interface          │  eviction policy pluggable│
│  Decorator ⭐    │  ThreadSafeLRUCache             │  thread-safety wrapper   │
│  Facade          │  CacheService                   │  cache + stats ek jagah  │
│  (technique)     │  LRUCacheCore                   │  O(1) list + splice      │
│  (technique)     │  CacheStatistics                │  atomic lock-free counters│
└──────────────────┴─────────────────────────────────┴─────────────────────────┘
```

---

## 1. Strategy Pattern — `ICache` interface

### 📌 Concept
Cache ke operations (get/put/...) ka ek generic **contract** define karo, "kaise" ko concrete
classes pe chhod do. Isse eviction policy plug-and-play ban jaati hai.

### PATTERN KE ROLES
- **Strategy interface** → [`ICache<Key, Value>`](./cache/ICache.h)
- **Concrete strategies** → [`LRUCacheCore`](./cache/LRUCacheCore.h), [`ThreadSafeLRUCache`](./cache/ThreadSafeLRUCache.h) (aur sibling folder ka `LFUCacheCore`)

### ⭐ KYUN?
> Client sirf `ICache<K,V>*` pakadta hai — usse pata nahi andar LRU hai ya LFU. Is repo me
> `LRU_Cache_LLD` aur `LFU_Cache_LLD` **same `ICache`** implement karte hain, sirf algorithm alag.
> Policy swap karni ho to concrete class badlo — client untouched. **Open/Closed Principle.**

---

## 2. Decorator Pattern — `ThreadSafeLRUCache` ⭐

### 📌 Concept
Ek object ko badle bina uske upar nayi zimmedari "wrap" karo. Wrapper wahi interface implement
karta hai, to bahar se same lagta hai — bas ek super-power extra.

### PATTERN KE ROLES
- **Component (interface)** → `ICache`
- **Concrete component** → `LRUCacheCore` (pure algorithm, locks se anjaan)
- **Decorator** → [`ThreadSafeLRUCache`](./cache/ThreadSafeLRUCache.h) — har call ko mutex ke peeche wrap

### ⭐ KYUN? (Why Decorator over baking locks into core)
> `LRUCacheCore` sirf LRU algorithm janta hai (SRP). Locking ek **alag concern** hai jise
> decorator add karta hai. Single-thread me core seedha use karo (bina lock overhead),
> multi-thread me wrapper. Ek algorithm dono jagah reuse — koi duplication.

### ⚠ Sabse important insight: `get()` bhi lock leta hai
> LRU me get() entry ko front (MRU) pe **splice** karta hai — yaani cache ki order MUTATE
> hoti hai. To get bhi ek WRITE hai! Isi liye `shared_mutex` (parallel reads) seedhe kaam
> nahi karta. Ek "read-only peek" API (jo recency na badle) ho tab shared_mutex use ho sakta —
> ye ek extension idea hai.

---

## 3. Facade Pattern — `CacheService`

### 📌 Concept
Multiple cheezon (cache + statistics) ko ek patle interface ke peeche jodo.

### Implementation
[`CacheService`](./core/CacheService.h) har operation ko cache pe forward karta hai **aur**
saath me statistics update kar deta hai:
```cpp
service.get(key);  // andar: cache.get() + (hit? recordHit : recordMiss)
service.put(k, v); // andar: cache.put() + recordPut + (evicted? recordEviction)
```

### ⭐ KYUN?
> Metrics ek "cross-cutting" concern hai — cache algorithm me ghusaane ke bajaye service layer
> pe wrap kiya. Cache core apne kaam pe focus rehta, aur wahi `CacheStatistics` LFU folder me bhi reuse hoti.

---

## 4. Technique: O(1) LRU — `list` + `map` + `splice` ⭐

Ye GoF pattern nahi, par is folder ka **asli dil** hai (LRUCacheCore.h):

| Structure | Kaam |
|-----------|------|
| `order_` : `std::list<CacheEntry>` | recency order (front=MRU, back=LRU) |
| `lookup_` : key → list **iterator** | O(1) lookup (list scan nahi) |

- **Access (touch):** `order_.splice(begin, order_, it)` — entry ko front pe le jao, **O(1)**, bina copy/move.
- **Evict:** `order_.pop_back()` — sabse purana (LRU) hatao, **O(1)**.

> **⭐ `splice` iterators ko INVALIDATE nahi karta** — isi liye `lookup_` me stored iterator
> splice ke baad bhi valid rehta hai. Yahi wajah hai ki `list` use hui, `vector` nahi (vector
> me element move sab iterators tod deta). Ye LRU implementation ka sabse nazuk C++ detail hai.

**LRU vs LFU:** LRU entry me sirf key+value hoti hai (recency list-order me implicit hai).
LFU node me frequency + bucket-iterator bhi chahiye. Isliye LRU **simpler** hai — kam moving parts,
kam bug-surface. (Detailed comparison: [`WHY_LRU_OVER_LFU.md`](./WHY_LRU_OVER_LFU.md).)

---

## 5. Technique: Lock-free atomic statistics

[`CacheStatistics`](./stats/CacheStatistics.h) me counters `std::atomic<long long>` hain.
Threads bina mutex ke `fetch_add` se safely count karte hain. `memory_order_relaxed` — kyunki
ye **sirf counters** hain (synchronization ke liye nahi), sabse sasta atomic mode.

---

## Object Relationships (UML samajhne ke liye)

```
CacheService      ◆──  ThreadSafeLRUCache   (Composition: service owns cache)
CacheService      ◆──  CacheStatistics      (Composition)
ThreadSafeLRUCache ◆── LRUCacheCore          (Composition: decorator wraps core)
ThreadSafeLRUCache ─▷  ICache                (implements)
LRUCacheCore      ─▷   ICache                (implements)
LRUCacheCore      ◆──  CacheEntry (order_)   (Composition: list owns entries)
lookup_           ··▶  order_ (iterators)    (Association: iterators point into list)
```
- `◆──` Composition (owns) · `─▷` Inheritance (implements) · `··▶` weak Association (iterator)

---

## Requirement → Pattern/Technique quick map

| Requirement | Kaise pura hua |
|-------------|----------------|
| Positive capacity | `CacheConfig` constructor/setter validation (fail-fast) |
| get hit → value + promote MRU | `LRUCacheCore::get` + `touch`/`splice` |
| get miss → nullopt | `std::optional` return |
| put insert/update + promote | `LRUCacheCore::put` |
| evict LRU exactly once | `evictLeastRecentlyUsed` (`pop_back`) |
| contains without recency change | `contains` (touch nahi karta) |
| statistics | `CacheStatistics` atomic counters |
| thread safety | **Decorator** `ThreadSafeLRUCache` (mutex) |
| average O(1) | `list` + `map` + stored iterator + `splice` |
| concurrency stress demo | `main.cpp` (16 threads + barrier + latch) |

---

## "Ye pattern kyun NAHI use kiya" (interview me pooche jaate hain)

- **`shared_mutex` (reader-writer lock) get() ke liye?** Nahi — LRU me get() recency ko
  **mutate** karta hai (splice to front), to wo shared (read) lock nahi le sakta. Read-only
  "peek" API ho to alag baat.
- **Factory for policy selection?** Abhi ek hi policy (LRU) is folder me. Sibling folder me
  LFU alag hai. Multiple policies ek jagah se chahiye ho tab factory — abhi YAGNI.
- **Singleton cache?** Cache inject hoti hai (`CacheService` me). Singleton global state deta
  jo testing mushkil karta — zaroorat na ho to mat lagao.
- **`vector` instead of `list`?** Splice/erase-at-position O(1) chahiye aur iterators stable
  rehne chahiye — `vector` dono nahi de sakta (move sab iterators tod deta). Isliye `list`.
- **Sharded locks abhi kyun nahi?** Ek mutex = simple + **sahi** (correctness first). High
  concurrency pe `hash(key) % N` sharding se scale — natural extension, abhi over-engineering.
```

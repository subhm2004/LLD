# Design Patterns Used — Concurrent HashMap LLD

Ye ek **thread-safe key-value map** hai jo do concurrency approaches deta hai. Design ka
core ek behavioural pattern (Strategy) hai, plus kuch important concurrency techniques —
lock striping (fine-grained locking), lock-free atomic statistics, aur ek default-selection
type alias.

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                            PATTERN OVERVIEW                                    │
├──────────────────┬─────────────────────────────────┬─────────────────────────┤
│  Pattern         │  Kahan (class)                  │  Kis liye                │
├──────────────────┼─────────────────────────────────┼─────────────────────────┤
│  Strategy ⭐     │  IConcurrentMap + 2 impls       │  concurrency approach swap│
│  Type alias      │  ConcurrentHashMap = Striped    │  "default choice" ek naam │
│  (technique)     │  StripedHashMap (lock striping) │  fine-grained parallelism │
│  (technique)     │  MapStatistics (atomics)        │  lock-free cross-stripe   │
│                  │                                 │  counters                 │
└──────────────────┴─────────────────────────────────┴─────────────────────────┘
```

---

## 1. Strategy Design Pattern ⭐ — `IConcurrentMap`

### 📌 Concept
Thread-safe map ke operations (put/get/remove/...) ka ek common **contract** define karo,
aur "thread-safety kaise deni hai" (ek bada lock ya kai chhote) ko concrete classes pe chhod do.

### PATTERN KE ROLES
- **Strategy interface** → [`IConcurrentMap`](./core/IConcurrentMap.h)
- **Concrete strategies**:
  - [`CoarseGrainedHashMap`](./core/CoarseGrainedHashMap.h) — poore map pe **1 mutex**
  - [`StripedHashMap`](./core/StripedHashMap.h) — **N locks** (lock striping)

### Implementation
[`main.cpp`](./main.cpp) me `demo_basic(IConcurrentMap&)` aur `benchmark_puts(IConcurrentMap&)`
dono strategies pe chalte hain — ek hi client code, alag-alag locking algorithm:
```cpp
StripedHashMap striped(16);      demo_basic(striped);   // andar: fine-grained
CoarseGrainedHashMap coarse;     demo_basic(coarse);    // andar: 1 mutex
```

### ⭐ KYUN? (Why Strategy)
> Workload alag, best approach alag. Kam load / simplicity chahiye -> Coarse. High concurrency,
> alag keys pe parallel kaam -> Striped. Client ko sirf `IConcurrentMap` dikhta hai — approach
> swap karo, client code untouched. **Open/Closed Principle.**

---

## 2. Type alias — `ConcurrentHashMap` (default selection)

[`ConcurrentHashMap.h`](./core/ConcurrentHashMap.h) ek line ka alias hai:
```cpp
using ConcurrentHashMap = StripedHashMap; // default = lock striping
```
> **KYUN?** "Default best choice" ko ek saaf naam mil gaya. Client `ConcurrentHashMap` likhe,
> use andar ki detail (striping) jaanne ki zaroorat nahi. Kal ko default badalna ho, bas ye ek
> line badlo — poora codebase nayi implementation use karne lagega. Ek jagah ka switch.

---

## 3. Technique: Lock Striping (fine-grained locking) ⭐ — is folder ka dil

### 📌 Concept
Ek bade lock ki jagah, map ko **N stripes** me baanto — har stripe ka apna mutex + apna chhota
bucket. Key kaunse stripe me jaayegi = `hash(key) % N`.

```
CoarseGrained:  [ ek mutex ] -> poora map           -> ek waqt me 1 thread
Striped:        stripe0[mtx0|bucket0]  stripe3[mtx3|bucket3]  stripe9[mtx9|bucket9]
                "apple"->s3 (lock3)    "mango"->s9 (lock9)   -> DONO parallel chal sakte!
```

### ⭐ Coarse-grained vs Lock striping (core interview comparison)
| Cheez | CoarseGrained (1 mutex) | StripedHashMap (N locks) |
|-------|--------------------------|---------------------------|
| Correctness | ✅ sahi | ✅ sahi |
| Alag keys parallel? | ❌ nahi (ek lock, serialize) | ✅ haan (alag stripes = alag locks) |
| Same key parallel? | ❌ | ❌ (same stripe = same lock) |
| Code complexity | simple | thodi zyada |
| Memory | 1 mutex | N mutex + N bucket |
| Best for | kam load / simplicity | high concurrency, multi-key |

> **⚠ Striping "poora" parallel nahi deta** — bas contention ko N stripes me BAANT deta hai.
> Same key (ya same stripe) pe threads phir bhi serialize honge. Fayda tab jab keys alag-alag
> stripes me girein (jo aam workload me hota hai).

### ⭐ `vector<unique_ptr<mutex>>` kyun (seedha `vector<mutex>` nahi)?
> `std::mutex` non-copyable + non-movable hai. `vector<mutex>` resize/reallocate karega to
> elements ko move karna padega -> compile error. `unique_ptr<mutex>` me vector sirf POINTERS
> move karta hai (mutex khud apni jagah stable) — ye classic "mutex ko container me kaise rakhein" solution.

---

## 4. Technique: Lock-free atomic statistics ⭐

[`MapStatistics`](./stats/MapStatistics.h) ke counters `std::atomic<long long>` hain.

> **Yahan atomic SACH me zaroori hai (optional nahi):** StripedHashMap me alag stripes ke alag
> locks hote hain. Do threads jo ALAG stripes me kaam kar rahe hain, wo ek saath (bina kisi
> COMMON lock ke) `stats_.recordPut()` bula sakte hain. Plain `long long` hota to `++` (read+write)
> takra ke count galat karta (lost update). `atomic::fetch_add` isse ek indivisible step banata —
> bina lock ke sahi ginti. Yaani: **data per-stripe lock se safe, par shared stats khud atomic**.

---

## Object Relationships (UML samajhne ke liye)

```
CoarseGrainedHashMap  ─▷  IConcurrentMap          (implements)
StripedHashMap        ─▷  IConcurrentMap          (implements)
CoarseGrainedHashMap  ◆──  mutex + unordered_map + MapStatistics   (Composition)
StripedHashMap        ◆──  N × (mutex + bucket) + MapStatistics    (Composition)
ConcurrentHashMap     ══   StripedHashMap          (type alias)
```
- `─▷` Inheritance (implements) · `◆──` Composition (owns) · `══` alias

---

## Requirement → Pattern/Technique quick map

| Requirement | Kaise pura hua |
|-------------|----------------|
| IConcurrentMap interface | **Strategy** interface |
| CoarseGrained (1 mutex) | `CoarseGrainedHashMap` |
| Lock striping (N stripes) | `StripedHashMap` + `hash % N` |
| ConcurrentHashMap alias → striped | type alias (default selection) |
| statistics (gets/hits/misses/puts) | `MapStatistics` atomic counters |
| multi-thread read/write safe | per-map / per-stripe mutex |
| max parallelism (different stripes) | **lock striping** |
| same key -> last-writer-wins | `map[key] = value` under stripe lock |
| C++17 (optional/hash/unordered_map) | throughout |

> **Cleanup + fix notes:**
> 1. 🧹 `ConcurrentHashMap.h` (aur `main.cpp`) me bekaar `#include <bits/stdc++.h>` tha —
>    alias file ko sirf `StripedHashMap.h` chahiye tha. **Hata diya** (portable, targeted includes).
> 2. 🐛 **Data race fix (demo):** `demo_concurrent_reads` me teen reader threads bina lock ke
>    `std::cout << "[reader N] done"` kar rahe the. `std::cout` **concurrent formatted output ke
>    liye thread-safe nahi** — stream ki internal state (width/flags) pe race hoti hai
>    (ThreadSanitizer ne pakda). Cout ko ek `mutex` se guard kar diya. (Map bilkul theek tha —
>    ye sirf logging ki race thi.) Ab poora TSan-clean.

---

## "Ye pattern kyun NAHI use kiya" (interview me pooche jaate hain)

- **`shared_mutex` per stripe (parallel reads)?** Achha extension hai (read-heavy stripes ke
  liye), par is design me get() bhi `stats_` update karta hai (write) -> pure read nahi. Read-only
  "peek" API + shared_mutex se parallel reads mil sakte — requirements me listed extension. Abhi
  simple exclusive lock (correctness first).
- **Lock-free (CAS / atomic linked-list buckets)?** Fastest, par likhna aur SAHI rakhna bahut
  mushkil (ABA problem, memory reclamation). Interview me mention karo; production me `folly::
  ConcurrentHashMap` / `java.util.concurrent.ConcurrentHashMap` jaisi tested libraries use karo.
- **Decorator (jaise cache folders me ThreadSafe wrapper)?** Wahan thread-safety ek WRAPPER thi
  (core alag). Yahan thread-safety har implementation ke ANDAR baked hai (kyunki striping ke liye
  lock aur data ko saath design karna padta) — isliye Strategy zyada natural hai, Decorator nahi.
- **`vector<mutex>` seedha?** Compile hi nahi hoga (mutex non-movable) — `unique_ptr<mutex>` chahiye (upar note).
- **`size()` ka atomic global counter?** Rakh sakte the, par har put/remove pe ek aur atomic
  update = overhead, aur concurrent map me exact instantaneous size waise bhi meaningless. Isliye
  best-effort per-stripe sum (jab chahiye tab compute).
```

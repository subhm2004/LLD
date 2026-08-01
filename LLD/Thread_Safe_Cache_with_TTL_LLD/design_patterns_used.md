# Design Patterns Used in Thread Safe Cache with TTL LLD

Thread Safe Cache with TTL LLD system me high concurrency locking, cache evictions policies, statistics metrics logging, aur lazy cleanup strategies ko manage karne ke liye niche diye gaye patterns ka use kiya gaya hai:

---

## 1. Facade Design Pattern

### Concept
Facade pattern system complexity aur internal data-flow structures ko hide karke clients ke samne ek single simple public API entrance structure expose karta hai.

### Implementation
- [ThreadSafeTTLCache](file:///Users/shubham/Desktop/LLD/Thread_Safe_Cache_with_TTL_LLD/core/ThreadSafeTTLCache.h) class main facade interface ki tarah kaam karti hai.
- Client applications direct multiple operations tracking (jaise lazy evaluation pruning, cache hit/miss statistics calculations, active eviction limits checks, read-write locking synchronization) manually handle nahi karti.
- Client directly facade class ke generic simple methods call karta hai:
  - `put(...)`
  - `get(...)`
  - `remove(...)`
  - `cleanupExpired()`

---

## 2. Reader-Writer Lock Pattern (Concurrency Optimization)

### Concept
Concurrency control optimizations ke liye reads and writes blocks ko balance kiya jata hai. Multiple reader threads ko parallel read access diya jata hai, par writer thread ko complete exclusive access limit lock diya jata hai.

### Implementation
- C++17 [std::shared_mutex](file:///Users/shubham/Desktop/LLD/Thread_Safe_Cache_with_TTL_LLD/core/ThreadSafeTTLCache.h#L9) (`mtx_`) ka use kiya gaya hai.
- Read-only operations (`get`, `contains`, `size`) me `std::shared_lock` use hota hai, jisse multiple threads ek sath bina kisi delay ke check aur query kar sakte hain (High throughput reads).
- Modifying operations (`put`, `remove`, `cleanupExpired`) me `std::unique_lock` use hota hai jo write safety locks guarantee karta hai.

---

## 3. Lazy Evaluation & Eviction Pattern

### Concept
Resource processing ko delay kiya jata hai jab tak object utility strictly required na ho, jisse CPU overhead reduce hota hai.

### Implementation
- **Lazy Expiry**: `get()` call hone par check kiya jata hai ki key expire hui hai ya nahi. Agar key expired hai, toh use tabhi block level par erase (`lazyEraseAndMiss`) kiya jata hai.
- **On-Demand Eviction**: Naya item insert karte waqt (`put`) agar size capacity exceed karti hai, tabhi cleanup (`evictIfNeededLocked`) run hota hai.

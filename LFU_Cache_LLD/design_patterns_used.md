# Design Patterns Used in LFU Cache LLD

LFU Cache LLD system me thread safety locks mechanism, cache eviction policy contracts, aur interface coordination ko manage karne ke liye niche diye gaye patterns ka use kiya gaya hai:

---

## 1. Proxy / Decorator Design Pattern (Thread-Safety Wrapper)

### Concept
Proxy pattern ek handler or surrogate object provides karta hai jo original object ke access ko control aur intercept karta hai (jaise logging, authentication, ya locking/synchronization add karna).

### Implementation
- [ThreadSafeLFUCache](file:///Users/shubham/Desktop/LLD/LFU_Cache_LLD/cache/ThreadSafeLFUCache.h) class raw LFU core implementation [LFUCacheCore](file:///Users/shubham/Desktop/LLD/LFU_Cache_LLD/cache/LFUCacheCore.h) ke aage **Thread-Safe Proxy** (ya decorator) ki tarah kaam karti hai.
- `LFUCacheCore` sirf basic LFU updates (doubly linked list nodes shifting, frequency map modifications) handles karti hai bina locks coordinate kiye.
- `ThreadSafeLFUCache` sabhi methods (`get`, `put`, `remove`, `clear`) ko intercept karti hai aur internally mutex locks (`std::lock_guard<std::mutex>`) add karti hai, jisse multi-threaded race conditions block ho jati hain.

---

## 2. Strategy Design Pattern

### Concept
Strategy design pattern algorithms aur data operations interfaces ko define aur decouple karta hai taaki runtime par context dynamic classes switch/inject ho sakein.

### Implementation
- [ICache](file:///Users/shubham/Desktop/LLD/LFU_Cache_LLD/cache/ICache.h) cache operations ka main generic abstract strategy interface hai.
- Is interface ke templates use karke future me hum dynamic eviction strategies (jaise LRU, FIFO, LFU) implement kar sakte hain aur client code bina modification ke unhe consume kar sakta hai.

---

## 3. Facade Design Pattern

### Concept
Facade pattern subsystems ki details aur config settings ko simplify karke single utility endpoints provide karta hai.

### Implementation
- [CacheService](file:///Users/shubham/Desktop/LLD/LFU_Cache_LLD/core/CacheService.h) facade class ka work karti hai jo caching configuration, logs stats, aur active thread safe caches ko coordinate karti hai.

# Thread-Safe LRU Cache LLD

Interview-grade LRU cache implementation in C++17 with modular headers and concurrency demo.

## Folder Structure

```
LRU_Cache_LLD/
├── cache/
│   ├── ICache.h                 # Cache interface
│   ├── LRUCacheCore.h           # Single-threaded LRU (hash map + list)
│   └── ThreadSafeLRUCache.h     # Mutex-protected wrapper
├── config/
│   └── CacheConfig.h            # Capacity config
├── core/
│   └── CacheService.h           # Facade + statistics
├── enums/
│   └── CacheOperationType.h
├── models/
│   └── CacheEntry.h
├── stats/
│   └── CacheStatistics.h
├── utils/
│   └── ConcurrencyHelpers.h     # Barrier + latch for stress test
├── main.cpp
├── problem_statement.md
└── requirements.md
```

## Design Patterns

| Pattern | Where |
|---------|-------|
| **Facade** | `CacheService` |
| **Decorator** | `ThreadSafeLRUCache` wraps `LRUCacheCore` |
| **Strategy-ready interface** | `ICache` for future eviction policies |

## Build & Run

```bash
cd LRU_Cache_LLD
g++ -std=c++17 -pthread main.cpp -o lru_cache_app
./lru_cache_app
```

## Demo Scenarios in `main.cpp`

1. **Basic LRU** — eviction + hit/miss behavior
2. **Deterministic concurrency** — two threads interleaving puts/gets
3. **Stress test** — 16 threads × 500 operations

## Complexity

| Operation | Time |
|-----------|------|
| `get` | O(1) average |
| `put` | O(1) average |
| `remove` | O(1) average |

## Interview Talking Points

- Why one `mutex` (because `get` mutates recency order)
- How `list::splice` gives O(1) promote-to-front
- How map stores iterator for O(1) lookup
- Difference between `LRUCacheCore` and `ThreadSafeLRUCache`

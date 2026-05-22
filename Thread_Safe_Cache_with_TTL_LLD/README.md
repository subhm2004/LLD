# Thread-Safe Cache with TTL — LLD (C++17)

In-memory **key-value cache** with **per-entry TTL** and **thread-safe** concurrent access.

## Quick run

```bash
cd Thread_Safe_Cache_with_TTL_LLD
./compile.sh
./cache_ttl_app
```

## Structure

```
Thread_Safe_Cache_with_TTL_LLD/
├── core/ThreadSafeTTLCache.h   # Facade — put, get, cleanup, eviction
├── models/CacheEntry.h         # value + expires_at
├── stats/CacheStatistics.h
├── main.cpp                    # 5 concurrency demos
├── problem_statement.md
├── requirements.md
└── THREAD_SAFE_TTL_CACHE_COMPLETE.md
```

## API (core)

```cpp
ThreadSafeTTLCache cache(max_entries);

cache.put("user:1", "Alice", ttl_seconds);
auto v = cache.get("user:1");   // optional<string>
cache.cleanupExpired();
cache.size();                   // non-expired count
```

## Synchronization strategy

| Operation | Lock | Why |
|-----------|------|-----|
| `get` (hit) | `shared_lock` | Parallel reads |
| `get` (expired) | `shared_lock` then `unique_lock` | Lazy delete |
| `put` / `remove` / `cleanup` | `unique_lock` | Map mutation |

**Pattern:** Reader-Writer — see [`../Multi_threading_C++/Concurrency_Patterns/Reader_Writer_Pattern/`](../Multi_threading_C++/Concurrency_Patterns/Reader_Writer_Pattern/)

## Related projects

| Project | Relation |
|---------|----------|
| [`LRU_Cache_LLD`](../LRU_Cache_LLD/) | Eviction by recency, no TTL |
| [`Rate_Limiter_LLD`](../Rate_Limiter_LLD/) | Time windows, different problem |
| [`Multi_threading_C++/Concurrency_Challenges/`](../Multi_threading_C++/Concurrency_Challenges/) | Deadlock, livelock |

## Full design doc

[`THREAD_SAFE_TTL_CACHE_COMPLETE.md`](./THREAD_SAFE_TTL_CACHE_COMPLETE.md) — challenges, sequence flows, interview Q&A.

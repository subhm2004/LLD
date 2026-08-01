# Problem Statement: Thread-Safe LRU Cache

Design a **Least Recently Used (LRU) cache** with **thread-safe** access for concurrent readers and writers.

## Functional Requirements

1. Cache must support fixed capacity `N`.
2. `get(key)` should return value if present and mark key as most recently used.
3. `put(key, value)` should insert/update key and mark it as most recently used.
4. When capacity is exceeded, evict the **least recently used** key.
5. Support `contains(key)`, `remove(key)`, and `clear()`.
6. All public operations must be safe under concurrent access from multiple threads.

## Non-Functional Requirements

1. `get` and `put` should be average **O(1)** time complexity.
2. Design should be modular and extensible (stats, alternate eviction policies later).
3. Use clear separation between core LRU logic and thread-safety layer.

## Out of Scope

- Distributed cache / sharding
- Persistence (disk/redis)
- TTL-based expiry (future extension)

## Design Objective

Build interview-ready C++ LLD with separate headers for config, models, cache core, thread-safe wrapper, statistics, and facade service.

# Problem Statement: Thread-Safe LFU Cache

Design a **Least Frequently Used (LFU)** cache with **thread-safe** access.

## Functional Requirements

1. Fixed capacity `N`.
2. `get(key)` returns value and increases key's access frequency.
3. `put(key, value)` inserts or updates; increases frequency on update.
4. When full, evict the key with **lowest frequency**.
5. On frequency tie, evict **least recently used** among tied keys.
6. Support `contains`, `remove`, `clear`.
7. All operations must be safe under concurrent access.

## Non-Functional Requirements

1. Average **O(1)** `get` and `put`.
2. Modular headers: core logic vs thread-safety vs facade.
3. Track hits, misses, evictions.

## Out of Scope

- LRU / ARC / hybrid policies (see [LRU_Cache_LLD](../LRU_Cache_LLD/))
- Distributed cache, persistence, TTL

## Design Objective

Interview-ready LFU with hash map + frequency buckets + mutex wrapper.

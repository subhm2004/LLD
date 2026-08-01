# Requirements: Thread-Safe LRU Cache

## Functional Requirements

1. Initialize cache with positive capacity.
2. `get(key)` returns optional value; on hit, promote key to most-recent.
3. `get(key)` returns empty optional on miss.
4. `put(key, value)` inserts or updates key and promotes to most-recent.
5. On insert beyond capacity, evict LRU entry exactly once.
6. `contains(key)` checks existence without changing recency.
7. `remove(key)` deletes key if present.
8. `clear()` removes all entries.
9. Track hits, misses, puts, evictions, and removes.

## Non-Functional Requirements

1. Thread-safe API using mutual exclusion (`std::mutex`).
2. Average O(1) `get` and `put` using hash map + doubly linked list.
3. Header-only modular design for interview clarity.
4. Include concurrency stress demo in `main.cpp`.

## Assumptions

1. In-memory single-process deployment.
2. Keys and values are copyable types.
3. No null keys.
4. Capacity remains fixed during a run (dynamic resize optional extension).

## Core Data Structures

- `std::unordered_map<Key, list::iterator>` for key lookup
- `std::list<CacheEntry<Key, Value>>` for recency order

## Extension Ideas

1. TTL per key
2. `shared_mutex` with custom read-only peek API (no recency update)
3. Shard-level locks for partitioned cache
4. LRU + LFU hybrid eviction policy

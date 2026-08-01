# Requirements: Thread-Safe LFU Cache

## Functional Requirements

1. Initialize with positive capacity.
2. `get` on hit: return value and increment frequency.
3. `get` on miss: return empty optional.
4. `put` on existing key: update value and increment frequency.
5. `put` on new key: insert at frequency 1; evict if over capacity.
6. Eviction: minimum frequency first; tie-break LRU within bucket.
7. `getFrequency(key)` for debugging and demos.
8. Statistics: hits, misses, puts, evictions, removes.

## Non-Functional Requirements

1. `std::mutex` for thread safety.
2. Average O(1) using:
   - `unordered_map<Key, Node>`
   - `unordered_map<int, list<Node*>>` frequency buckets
   - `minFreq` tracker
3. **ISO C++17 required** (`config/CppStandard.h` compile-time check).
4. Header-modular layout; no `bits/stdc++.h`.

## Assumptions

1. In-memory, single process.
2. Copyable keys and values.
3. `get` and `put` both count as access (frequency increases).

## Data Structures

| Structure | Purpose |
|-----------|---------|
| `keyMap_` | O(1) key lookup |
| `freqBuckets_[f]` | Nodes with frequency `f` |
| `minFreq_` | Eviction pointer |

## Extensions

1. TTL per key
2. `ICache` polymorphism with LRU/LFU factory
3. Sharded locks for higher concurrency

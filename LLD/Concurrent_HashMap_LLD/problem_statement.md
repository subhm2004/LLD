# Problem: Design Concurrent HashMap

Design a **thread-safe** key-value map supporting concurrent `put`, `get`, and `remove` without corrupting internal state.

## Functional requirements

- `put(key, value)` — insert or update
- `get(key)` — return value or miss
- `remove(key)` — delete if present
- `contains(key)`, `size()`

## Concurrency requirements

- Multiple threads read/write safely.
- Maximize parallelism when keys hash to **different** stripes.
- Define behavior for concurrent updates to the **same** key (last writer wins).

## Approaches to implement

1. **Coarse-grained** — single mutex for whole map  
2. **Lock striping** — array of mutexes + per-stripe buckets (default)  
3. **CAS / lock-free** — extension (not in v1 code)

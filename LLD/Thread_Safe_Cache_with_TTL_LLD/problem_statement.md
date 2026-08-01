# Problem: Thread-Safe Cache with TTL

Design an in-memory **key-value cache** where:

- Multiple threads can **read and write** safely.
- Each entry has a **TTL** (time-to-live); after expiry `get` returns miss.
- Expired entries may be removed **lazily** (on read) or via **background cleanup**.
- Optional **max size** with eviction when full.

## Functional requirements

| API | Behavior |
|-----|----------|
| `put(key, value, ttl_sec)` | Insert or update; set expiry = now + ttl |
| `get(key)` | Return value if present and not expired; else miss |
| `remove(key)` | Delete entry |
| `cleanupExpired()` | Sweep and remove all expired keys |
| `contains(key)` | True only if non-expired |
| `size()` | Count of non-expired entries |

## Concurrency requirements

- Concurrent `get` on different keys should scale (shared read lock).
- Concurrent `put` must not corrupt the map.
- No torn reads / stale value returned after expiry time passed.
- `cleanup` and `get` must not leave map in inconsistent state.

## Non-functional

- C++17, header-based LLD for interview clarity.
- Statistics: hits, misses, expired-on-get, cleanup sweeps.

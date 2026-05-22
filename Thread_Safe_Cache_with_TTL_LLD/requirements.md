# Requirements — Thread-Safe Cache with TTL

## Must have

- [x] `put` with per-key TTL
- [x] `get` returns `optional` — miss if missing or expired
- [x] Lazy expiry on `get` (erase expired entry)
- [x] `cleanupExpired()` bulk sweep
- [x] `shared_mutex` — parallel reads
- [x] `unique_lock` — writes, lazy erase, cleanup
- [x] Max entry cap with eviction
- [x] Thread-safe statistics
- [x] Demo: concurrent readers, concurrent writers, cleanup race

## Should have (extensions)

- [ ] Background cleanup thread on interval
- [ ] `get` with optional refresh TTL (sliding expiration)
- [ ] Namespace sharding (striped locks) for very high throughput
- [ ] Prometheus-style metrics export

## Concurrency challenges addressed

| Challenge | Approach in repo |
|-----------|------------------|
| Stale read after expiry | Check `expires_at` under lock before return |
| Put vs cleanup race | Both mutate map under exclusive lock |
| Concurrent put same key | `unique_lock` serializes |
| Read during modification | `shared_lock` for valid reads; writers block readers |

# 05 — Thread-Safe Cache (Read-Mostly)

**Source:** `../05_thread_safe_cache.cpp`  
**Binary:** `../bin/05_thread_safe_cache`

---

## Pattern context

**Classic RW use case:** cache — `get` bahut, `put` kam.

---

## `ThreadSafeCache`

| Method | Lock | Why |
|--------|------|-----|
| `get` | `shared_lock` | Parallel reads |
| `put` | `unique_lock` | Map structure change |
| `size` | `shared_lock` | Read metadata only |

---

## Demo flow

1. Preload `user:1`, `user:2`
2. 3 reader threads hammer `get("user:1")`
3. Main thread `put` update `user:1`
4. Readers eventually see `Alice-Updated`

---

## Advantages

| Fayda | Real system |
|-------|-------------|
| Scalable reads | Session/config cache |
| Safe writes | Invalidation exclusive |

---

## Run

```bash
./bin/05_thread_safe_cache
```

---

## Pitfall

`get` return karte waqt pointer/reference hold karo aur baad mein write ho — **stale data**. Copy return karo (jaise `optional<string>`).

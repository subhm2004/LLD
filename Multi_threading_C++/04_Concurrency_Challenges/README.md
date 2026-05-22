# Concurrency Challenges (C++17)

Classic concurrency **problems + fixes** — har topic alag folder, runnable code, detailed docs.

| Challenge | Folder | Focus |
|-----------|--------|-------|
| **Deadlock** | [`Deadlock/`](./Deadlock/) | Coffman conditions, 2-mutex deadlock, prevention |
| **Livelock** | [`Livelock/`](./Livelock/) | try_lock storms, backoff, vs deadlock |

## Build

```bash
cd Deadlock && ./compile.sh && ./bin/01_coffman_four_conditions
cd Livelock && ./compile.sh && ./bin/01_what_is_livelock
./run_all.sh   # inside each folder
```

## Suggested order

1. **Deadlock** — blocked forever  
2. **Livelock** — active but no progress (often after try_lock “fix” for deadlock)

## vs `02_Concurrency_Patterns/`

| Folder | Purpose |
|--------|---------|
| [`02_Concurrency_Patterns/`](../02_Concurrency_Patterns/) | Design patterns (pool, producer-consumer, …) |
| `04_Concurrency_Challenges/` | Pitfalls & fixes (deadlock, livelock, …) |

## Parent

[`../README.md`](../README.md) — full multi-threading module

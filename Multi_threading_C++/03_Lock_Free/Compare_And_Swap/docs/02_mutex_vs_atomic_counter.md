# 02 — Mutex vs Atomic Counter

**Source:** `../02_mutex_vs_atomic_counter.cpp`

4 threads × 250k increments — pehle `mutex`, phir `atomic::fetch_add`.

## Run

```bash
./bin/02_mutex_vs_atomic_counter
```

## Next

[`03_compare_exchange_strong_weak.md`](../03_compare_exchange_strong_weak.md)

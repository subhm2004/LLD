# Livelock — Concurrency Challenge (C++17)

Threads **chal rahi hain** (CPU busy) par **kaam complete nahi** ho raha — deadlock jaisa freeze nahi, par progress zero.

## Quick start

```bash
./compile.sh
./bin/01_what_is_livelock
./run_all.sh
```

## Theory (full)

[`LIVELOCK_COMPLETE.md`](./LIVELOCK_COMPLETE.md)

## Demos

| # | File | README | Topic |
|---|------|--------|-------|
| 01 | `01_what_is_livelock.cpp` | [`docs/01_what_is_livelock.md`](./docs/01_what_is_livelock.md) | Definition + analogy |
| 02 | `02_polite_try_lock_livelock.cpp` | [`docs/02_polite_try_lock_livelock.md`](./docs/02_polite_try_lock_livelock.md) | try_lock without backoff |
| 03 | `03_random_backoff_fix.cpp` | [`docs/03_random_backoff_fix.md`](./docs/03_random_backoff_fix.md) | Random delay fix |
| 04 | `04_exponential_backoff.cpp` | [`docs/04_exponential_backoff.md`](./docs/04_exponential_backoff.md) | 2^attempt ms |
| 05 | `05_corridor_yield_simulation.cpp` | [`docs/05_corridor_yield_simulation.md`](./docs/05_corridor_yield_simulation.md) | Corridor analogy |
| 06 | `06_compare_deadlock_livelock_starvation.cpp` | [`docs/06_compare_deadlock_livelock_starvation.md`](./docs/06_compare_deadlock_livelock_starvation.md) | 3-way compare |

## Learning order

`01 → 02` (problem) → `03` or `04` (fix) → `05` → `06`

## Related

- [`../Deadlock/`](../Deadlock/) — blocked forever vs active retry
- [`../../05_Classic_Problems/Deadlock_Legacy/deadlock_and_protection.cpp`](../../05_Classic_Problems/Deadlock_Legacy/deadlock_and_protection.cpp) — livelock comment block

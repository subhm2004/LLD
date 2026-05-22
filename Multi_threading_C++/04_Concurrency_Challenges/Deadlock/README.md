# Deadlock — Concurrency Challenge (C++17)

Do ya zyada threads **ek doosre ka wait** karti hain — koi aage nahi badhti → program hang.

## Quick start

```bash
./compile.sh
./bin/01_coffman_four_conditions
./run_all.sh
```

## Theory (full)

[`DEADLOCK_COMPLETE.md`](./DEADLOCK_COMPLETE.md)

## Demos

| # | File | README | Topic |
|---|------|--------|-------|
| 01 | `01_coffman_four_conditions.cpp` | [`docs/01_coffman_four_conditions.md`](./docs/01_coffman_four_conditions.md) | 4 zaroori conditions |
| 02 | `02_opposite_order_deadlock.cpp` | [`docs/02_opposite_order_deadlock.md`](./docs/02_opposite_order_deadlock.md) | A→B vs B→A (timed detect) |
| 03 | `03_fix_same_lock_order.cpp` | [`docs/03_fix_same_lock_order.md`](./docs/03_fix_same_lock_order.md) | Same order fix |
| 04 | `04_fix_std_lock.cpp` | [`docs/04_fix_std_lock.md`](./docs/04_fix_std_lock.md) | `std::lock` |
| 05 | `05_fix_scoped_lock.cpp` | [`docs/05_fix_scoped_lock.md`](./docs/05_fix_scoped_lock.md) | `scoped_lock` C++17 |
| 06 | `06_try_lock_livelock_starvation.cpp` | [`docs/06_try_lock_livelock_starvation.md`](./docs/06_try_lock_livelock_starvation.md) | try_lock, livelock |
| 07 | `07_optional_infinite_deadlock.cpp` | [`docs/07_optional_infinite_deadlock.md`](./docs/07_optional_infinite_deadlock.md) | Real hang (manual only) |

## Learning order

`01 → 02 → 03 → 04 → 05 → 06` — phir optional `07` manually.

## Related

- [`../../05_Classic_Problems/Deadlock_Legacy/deadlock_and_protection.cpp`](../../05_Classic_Problems/Deadlock_Legacy/deadlock_and_protection.cpp) — original combined file
- [`../../01_Fundamentals/lock_mechanism.cpp`](../../01_Fundamentals/lock_mechanism.cpp) — `std::lock`
- [`../../02_Concurrency_Patterns/`](../../02_Concurrency_Patterns/) — design patterns

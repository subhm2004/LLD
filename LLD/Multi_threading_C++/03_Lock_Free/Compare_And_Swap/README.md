# Compare-And-Swap (CAS) — C++17

Lock-free building block — **read, compare, conditional write** ek atomic step mein.  
Interview reference: [AlgoMaster — Compare-and-Swap](https://algomaster.io/learn/concurrency-interview/compare-and-swap)

## Quick start

```bash
./compile.sh
./bin/01_what_is_cas
./run_all.sh
```

## Theory (full)

[`COMPARE_AND_SWAP_COMPLETE.md`](./COMPARE_AND_SWAP_COMPLETE.md)

## Demos

| # | File | Doc | Topic |
|---|------|-----|-------|
| 01 | `01_what_is_cas.cpp` | [`docs/01_what_is_cas.md`](./docs/01_what_is_cas.md) | CAS definition, success/fail |
| 02 | `02_mutex_vs_atomic_counter.cpp` | [`docs/02_mutex_vs_atomic_counter.md`](./docs/02_mutex_vs_atomic_counter.md) | Mutex vs `fetch_add` |
| 03 | `03_compare_exchange_strong_weak.cpp` | [`docs/03_compare_exchange_strong_weak.md`](./docs/03_compare_exchange_strong_weak.md) | strong vs weak |
| 04 | `04_cas_spin_lock.cpp` | [`docs/04_cas_spin_lock.md`](./docs/04_cas_spin_lock.md) | Spin lock via CAS (`SpinLock.h`) |
| 05 | `05_aba_problem_and_version_stamp.cpp` | [`docs/05_aba_problem_and_version_stamp.md`](./docs/05_aba_problem_and_version_stamp.md) | ABA + version stamp |
| 06 | `06_memory_order_basics.cpp` | [`docs/06_memory_order_basics.md`](./docs/06_memory_order_basics.md) | relaxed / acquire-release |

## Learning order

`01 → 02 → 03 → 04 → 05 → 06`

Pehle **mutex + condition_variable** ([`../../02_Concurrency_Patterns/`](../../02_Concurrency_Patterns/)) — phir CAS (lock-free layer).

## Interview one-liners

| Topic | Line |
|-------|------|
| **CAS** | Atomically update only if memory still equals expected; else retry |
| **vs mutex** | CAS avoids blocking; contention still hits cache line |
| **weak vs strong** | weak can spuriously fail — use in loops; strong for one-shot |
| **ABA** | Pointer value same, structure changed — fix with version / hazard pointers |
| **spin lock** | CAS loop busy-waits — short critical sections only |

## Related

- [`../../01_Fundamentals/race_condition_and_synchronization.cpp`](../../01_Fundamentals/race_condition_and_synchronization.cpp)
- [`../../../Concurrent_HashMap_LLD`](../../../Concurrent_HashMap_LLD/) — lock striping (mutex-based)
- [`../../02_Concurrency_Patterns/Reader_Writer_Pattern/`](../../02_Concurrency_Patterns/Reader_Writer_Pattern/) — `shared_mutex`

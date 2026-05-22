# Multi-threaded Merge Sort (C++17)

Parallel merge sort — sequential baseline, thread-per-subtask, thread pool, fork-join (`std::async`).

## Quick start

```bash
./compile.sh
./bin/02_sequential_baseline
./bin/06_compare_timings
./run_all.sh
```

## Theory (full — ~500+ lines, interview deep dive)

[`MULTI_THREADED_MERGE_SORT_COMPLETE.md`](./MULTI_THREADED_MERGE_SORT_COMPLETE.md) — Amdahl, pool deadlock, har demo, Q&A, parallel merge, cheat sheet

## Demos

| # | Binary | Topic |
|---|--------|-------|
| 01 | `01_overview_parallelism` | Divide/merge, threshold, Amdahl |
| 02 | `02_sequential_baseline` | Classic merge sort |
| 03 | `03_thread_per_subtask` | `std::thread` per half (naive) |
| 04 | `04_thread_pool_threshold` | `SimpleThreadPool` + cutoff |
| 05 | `05_fork_join_async` | `std::async` fork-join |
| 06 | `06_compare_timings` | Same input, timing comparison |

## Headers

| File | Role |
|------|------|
| `MergeSortUtils.h` | `merge`, sequential sort, helpers |
| `SimpleThreadPool.h` | Reusable worker pool for demo 04 |

## Learning order

`01 → 02 → 04 → 05 → 06` — optional `03` to see thread explosion risk.

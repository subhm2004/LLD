# 06 — Interview Problems (C++17)

LeetCode concurrency + parallel algorithms + classic primitives.

**Full index:** [`INTERVIEW_PROBLEMS_COMPLETE.md`](./INTERVIEW_PROBLEMS_COMPLETE.md)

---

## Ordered printing (LeetCode)

| LC | Problem | Folder |
|----|---------|--------|
| 1114 | Print in Order | [`Print_in_Order/`](./Print_in_Order/) |
| 1115 | Print FooBar Alternately | [`Print_FooBar_Alternately/`](./Print_FooBar_Alternately/) |
| 1116 | Print Zero Even Odd | [`Print_Zero_Even_Odd/`](./Print_Zero_Even_Odd/) |
| 411 | Fizz Buzz Multithreaded | [`Fizz_Buzz/`](./Fizz_Buzz/) — [COMPLETE](./Fizz_Buzz/FIZZ_BUZZ_MULTITHREADED_COMPLETE.md) |
| 1117 | Building H2O | [`Building_H2O/`](./Building_H2O/) |
| 1226 | Dining Philosophers | [`Dining_Philosophers_LC1226/`](./Dining_Philosophers_LC1226/) |
| 1242 | Web Crawler Multithreaded | [`Web_Crawler_Multithreaded_LC1242/`](./Web_Crawler_Multithreaded_LC1242/) |

## Primitives & data structures

| Topic | Folder |
|-------|--------|
| Barrier + CountDownLatch | [`Barrier_Synchronization/`](./Barrier_Synchronization/) |
| Bounded blocking queue | [`Bounded_Blocking_Queue/`](./Bounded_Blocking_Queue/) |
| Shared semaphore | [`common/CountingSemaphore.h`](./common/CountingSemaphore.h) |

## Parallel algorithms

| Topic | Folder |
|-------|--------|
| Multi-threaded merge sort | [`Merge_Sort/`](./Merge_Sort/) — [COMPLETE](./Merge_Sort/MULTI_THREADED_MERGE_SORT_COMPLETE.md) |

---

## Quick start

```bash
../scripts/build_interview_problems.sh

# Highlights:
cd Fizz_Buzz && ./bin/04_condition_variable
cd Print_FooBar_Alternately && ./bin/01_semaphore_solution
cd Building_H2O && ./bin/01_bond_molecules
cd Barrier_Synchronization && ./bin/01_barrier_demo
cd Bounded_Blocking_Queue && ./bin/01_producer_consumer_demo
cd Dining_Philosophers_LC1226 && ./bin/01_problem_overview
cd Web_Crawler_Multithreaded_LC1242 && ./bin/03_multithreaded_crawl
cd Merge_Sort && ./bin/06_compare_timings
```

---

## Learning path

```
Print_in_Order → Print_FooBar → Zero_Even_Odd → Fizz_Buzz → H2O
        → Bounded_Queue → Barrier → Dining_Philosophers → Web_Crawler → Merge_Sort
```

## Prerequisites

- [`../01_Fundamentals/`](../01_Fundamentals/)
- [`../02_Concurrency_Patterns/Signaling_Pattern/`](../02_Concurrency_Patterns/Signaling_Pattern/)

# Interview Problems — Complete Index (C++17)

All problems under [`06_Interview_Problems/`](./) — LeetCode concurrency + parallel algorithms.

---

## Ordered printing family (LeetCode)

| LC # | Problem | Folder | Core primitive |
|------|---------|--------|----------------|
| 1114 | Print in Order | [`Print_in_Order/`](./Print_in_Order/) | CV pipeline / semaphores |
| 1115 | Print FooBar Alternately | [`Print_FooBar_Alternately/`](./Print_FooBar_Alternately/) | Ping-pong semaphores |
| 1116 | Print Zero Even Odd | [`Print_Zero_Even_Odd/`](./Print_Zero_Even_Odd/) | State machine + CV |
| 411 | Fizz Buzz Multithreaded | [`Fizz_Buzz/`](./Fizz_Buzz/) | 4-thread turn routing |
| 1117 | Building H2O | [`Building_H2O/`](./Building_H2O/) | Resource counters 2:1 |
| 1226 | Dining Philosophers | [`Dining_Philosophers_LC1226/`](./Dining_Philosophers_LC1226/) | Ordered forks / N−1 seats / try_lock |
| 1242 | Web Crawler Multithreaded | [`Web_Crawler_Multithreaded_LC1242/`](./Web_Crawler_Multithreaded_LC1242/) | BFS + thread pool + visited set |

**Pattern:** `mutex` + `condition_variable` + predicate, or **counting semaphores** for turn-taking.

---

## Synchronization primitives

| Topic | Folder |
|-------|--------|
| Barrier (cyclic) | [`Barrier_Synchronization/`](./Barrier_Synchronization/) |
| CountDownLatch | same folder (`02_countdown_latch.cpp`) |
| Bounded blocking queue | [`Bounded_Blocking_Queue/`](./Bounded_Blocking_Queue/) |

---

## Parallel algorithms & graph crawl

| Topic | Folder |
|-------|--------|
| Multi-threaded merge sort | [`Merge_Sort/`](./Merge_Sort/) |
| Web crawler (multithreaded BFS) | [`Web_Crawler_Multithreaded_LC1242/`](./Web_Crawler_Multithreaded_LC1242/) |

## Resource & deadlock classics

| Topic | Folder |
|-------|--------|
| Dining philosophers (LC API) | [`Dining_Philosophers_LC1226/`](./Dining_Philosophers_LC1226/) |
| Long-form DP demo (4 solutions) | [`../../05_Classic_Problems/Dining_Philosophers/`](../../05_Classic_Problems/Dining_Philosophers/) |

---

## Shared helper

[`common/CountingSemaphore.h`](./common/CountingSemaphore.h) — C++17 semaphore (used by FooBar, Print in Order).

---

## Suggested study order

1. `Print_in_Order` → `Print_FooBar_Alternately` (simplest turns)
2. `Print_Zero_Even_Odd` → `Fizz_Buzz` (harder routing)
3. `Building_H2O` (resource bonding)
4. `Bounded_Blocking_Queue` → link to Producer-Consumer pattern
5. `Barrier_Synchronization`
6. `Dining_Philosophers_LC1226` → compare `05_Classic_Problems`
7. `Web_Crawler_Multithreaded_LC1242`
8. `Merge_Sort` (performance / pool)

---

## Build all interview problems

```bash
cd ..
./scripts/build_interview_problems.sh
```

Or from repo: [`../scripts/build_all.sh`](../scripts/build_all.sh)

---

## Prerequisites

- [`../01_Fundamentals/`](../01_Fundamentals/)
- [`../02_Concurrency_Patterns/Signaling_Pattern/`](../02_Concurrency_Patterns/Signaling_Pattern/)

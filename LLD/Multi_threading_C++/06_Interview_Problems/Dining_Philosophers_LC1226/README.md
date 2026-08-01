# Dining Philosophers — LeetCode 1226

Five philosophers share five forks; `wantsToEat()` must be thread-safe.

| # | File | Solution |
|---|------|----------|
| 01 | `01_problem_overview.cpp` | **Ordered forks** (lock min index first) — default `wantsToEat` |
| 02 | `02_waiter_n_minus_one.cpp` | **N−1 seats** semaphore |
| 03 | `03_try_lock_backoff.cpp` | **try_lock** + random backoff |

## Run

```bash
./compile.sh && ./bin/01_problem_overview
```

## API (`DiningPhilosophers.h`)

Matches LC 1226 callbacks: `pickLeftFork`, `pickRightFork`, `eat`, `putLeftFork`, `putRightFork`.

## Classic monolith (4 versions)

Longer Hinglish demo: [`../../05_Classic_Problems/Dining_Philosophers/dining_philosophers.cpp`](../../05_Classic_Problems/Dining_Philosophers/dining_philosophers.cpp)

## Interview one-liner

Circular wait on forks → break with **total ordering** of mutexes, **limit diners to N−1**, or **try_lock** with backoff.

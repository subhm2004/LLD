# 01 — Fundamentals (C++17)

Threads, mutex, locks, race conditions, semaphores — **pehle yahan se shuru karo**.

## Files (recommended order)

| # | File | Topic |
|---|------|-------|
| 1 | `lessson_1_join.cpp` | `std::thread`, `join`, `detach` |
| 2 | `race_condition_and_synchronization.cpp` | Data race → mutex fix |
| 3 | `lesson_2_locks_and_mutex.cpp` | Basic mutex |
| 4 | `types_of_locks.cpp` | `lock_guard`, `unique_lock`, `try_lock` |
| 5 | `lock_mechanism.cpp` | RAII locks, `std::lock` |
| 6 | `lesson_3.cpp` | `condition_variable` |
| 7 | `semaphor.cpp` | Semaphore + connection pool |
| 8 | `execution_time_of_code.cpp` | `chrono` benchmarking |
| 9 | `Thread_Safe_Injection.cpp` | Thread-safe DI |

## Build all

```bash
./compile.sh
./bin/lessson_1_join
```

## Next

[`../02_Concurrency_Patterns/`](../02_Concurrency_Patterns/) — signaling, producer-consumer, thread pool, reader-writer

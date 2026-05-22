# Print FooBar Alternately — LeetCode 1115

Two threads print **foo** and **bar** alternately, `n` times each → `foobar` × n.

| # | File | Approach |
|---|------|----------|
| 01 | `01_semaphore_solution.cpp` | `fooSem(1)`, `barSem(0)` — **best for interview** |
| 02 | `02_mutex_cv_solution.cpp` | `bool foo_turn` + CV |

## Run

```bash
./compile.sh && ./bin/01_semaphore_solution
```

## Pattern

Ping-pong semaphores — same idea as turnstile / alternating workers.

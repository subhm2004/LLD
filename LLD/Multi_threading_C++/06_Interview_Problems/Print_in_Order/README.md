# Print in Order — LeetCode 1114

Three threads print **first → second → third** in strict order.

| # | File | Approach |
|---|------|----------|
| 01 | `01_problem_overview.cpp` | `mutex` + `condition_variable` chain |
| 02 | `02_semaphore_chain.cpp` | Two semaphores (pipeline) |

## Run

```bash
./compile.sh && ./bin/01_problem_overview
```

## Interview one-liner

Second thread `wait` until `first_done`; third waits until `second_done` — pipeline synchronization.

## Related

- [`../Print_FooBar_Alternately/`](../Print_FooBar_Alternately/) — LC 1115 (2-thread alternation)
- [`../Fizz_Buzz/`](../Fizz_Buzz/) — LC 411 (4-thread turn rules)

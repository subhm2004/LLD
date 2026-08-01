# Print Zero Even Odd — LeetCode 1116

Three threads: print **0**, then **even**, then **0**, then **odd**, …

Example `n=2` → `0102`

| # | File | Approach |
|---|------|----------|
| 01 | `01_condition_variable.cpp` | State machine `ZERO → EVEN/ODD → ZERO` |

## Run

```bash
./compile.sh && ./bin/01_condition_variable
```

## Interview tip

After zero, next turn depends on whether `next` value is even or odd — same family as Fizz Buzz turn routing.

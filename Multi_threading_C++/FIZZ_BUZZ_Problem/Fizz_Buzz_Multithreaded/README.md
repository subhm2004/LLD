# Fizz Buzz Multithreaded (LeetCode 411)

4 threads print `1..n` in order: **fizz** (÷3), **buzz** (÷5), **fizzbuzz** (÷15), else **number**.

## Quick start

```bash
./compile.sh
./bin/01_problem_overview
./bin/04_condition_variable
./run_all.sh
```

## Theory (full)

[`FIZZ_BUZZ_MULTITHREADED_COMPLETE.md`](./FIZZ_BUZZ_MULTITHREADED_COMPLETE.md)

## Demos

| # | Binary | Topic |
|---|--------|-------|
| 01 | `01_problem_overview` | Rules + example output |
| 02 | `02_naive_busy_wait` | Spin until my turn (high CPU) |
| 03 | `03_semaphore_controller` | C++17 `CountingSemaphore` turn chain |
| 04 | `04_condition_variable` | `mutex` + `cv` + predicate (**best**) |
| 05 | `05_leetcode_api` | `FizzBuzz.h` — LeetCode method signatures |

## Headers

| File | Role |
|------|------|
| `FizzBuzz.h` | LeetCode-style `fizz` / `buzz` / `fizzbuzz` / `number` |
| `CountingSemaphore.h` | C++17 binary semaphore for demo 03 |

## Learning order

`01 → 04 → 05` — optional `02` (why not spin), `03` (semaphore signaling).

## Expected output (n=15)

```
1,2,fizz,4,buzz,fizz,7,8,fizz,buzz,11,fizz,13,14,fizzbuzz,
```

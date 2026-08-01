# 04 — Exponential Backoff

**Source:** `../04_exponential_backoff.cpp`  
**Binary:** `../bin/04_exponential_backoff`

## Formula

`wait = min(200, 2^attempt)` ms

## Use case

Network retries, lock contention under load.

## Run

```bash
./bin/04_exponential_backoff
```

Watch increasing backoff lines.

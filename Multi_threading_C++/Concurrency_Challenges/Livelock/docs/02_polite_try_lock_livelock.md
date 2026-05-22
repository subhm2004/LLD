# 02 — Polite try_lock Livelock

**Source:** `../02_polite_try_lock_livelock.cpp`  
**Binary:** `../bin/02_polite_try_lock_livelock`

## Kya hota hai

1. `try_lock(A)` success  
2. `try_lock(B)` fail → **unlock A** (polite)  
3. `yield()` — **no sleep**  
4. Repeat up to 50 times  

## Observe

`total_attempts` bahut high — CPU busy, success maybe nahi.

## Why not deadlock

Threads rarely **block** on second lock — they release and retry.

## Run

```bash
./bin/02_polite_try_lock_livelock
```

Then compare `03_random_backoff_fix`.

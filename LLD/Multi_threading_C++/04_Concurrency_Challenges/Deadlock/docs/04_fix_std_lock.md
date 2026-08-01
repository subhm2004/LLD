# 04 — Fix: std::lock

**Source:** `../04_fix_std_lock.cpp`  
**Binary:** `../bin/04_fix_std_lock`

## Kya hota hai

`defer_lock` + `lock(la, lb)` — dono ek saath ya koi nahi.

## Breaks

**Hold-and-wait** — partial lock nahi rehta.

## Run

```bash
./bin/04_fix_std_lock
```

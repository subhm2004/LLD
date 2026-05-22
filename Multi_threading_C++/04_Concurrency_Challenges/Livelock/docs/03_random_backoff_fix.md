# 03 — Random Backoff Fix

**Source:** `../03_random_backoff_fix.cpp`  
**Binary:** `../bin/03_random_backoff_fix`

## Fix

After failed try: `sleep(random 5–80ms)` per thread with different RNG seed.

## Result

Usually **success=yes**, attempts << demo 02.

## Interview

> "Desynchronize retries so both threads don't collide every time."

## Run

```bash
./bin/03_random_backoff_fix
```

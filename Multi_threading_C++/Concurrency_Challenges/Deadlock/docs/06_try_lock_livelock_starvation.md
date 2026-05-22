# 06 — try_lock, Livelock, Starvation

**Source:** `../06_try_lock_livelock_starvation.cpp`  
**Binary:** `../bin/06_try_lock_livelock_starvation`

## try_lock rule

Second lock fail → **first unlock** → retry with delay.

## Three concepts

| Term | Meaning |
|------|---------|
| Deadlock | Blocked forever |
| Livelock | Busy retry, no progress |
| Starvation | One thread always loses |

## Run

```bash
./bin/06_try_lock_livelock_starvation
```

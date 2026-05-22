# 06 — Compare Deadlock, Livelock, Starvation

**Source:** `../06_compare_deadlock_livelock_starvation.cpp`  
**Binary:** `../bin/06_compare_deadlock_livelock_starvation`

## Contains

- Comparison table  
- Links to `../Deadlock/`  
- Mini starvation demo (high thread hogs mutex)

## Run

```bash
./bin/06_compare_deadlock_livelock_starvation
```

## Revision

| | Blocked? | CPU? | Fix |
|--|----------|------|-----|
| Deadlock | Yes | Low | Lock order |
| Livelock | No | High | Backoff |
| Starvation | Waiting | Varies | Fair lock |

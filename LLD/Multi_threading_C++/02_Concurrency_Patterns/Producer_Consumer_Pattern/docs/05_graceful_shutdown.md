# 05 — Graceful Shutdown

**Source:** `../05_graceful_shutdown.cpp`  
**Binary:** `../bin/05_graceful_shutdown`

---

## Pattern context

Pipeline band karna: **no new items**, **drain buffer**, consumers **clean exit**.

---

## Is file mein kya ho raha hai

- `BoundedBuffer<string>(5)`
- Producer: `job-1` … `job-6` enqueue
- `signal_shutdown()` — `shutdown_=true`, `notify_all` both CVs
- Consumer: remaining jobs process → `consume()` → `nullopt`
- `is_shutdown()` true at end

---

## `signal_shutdown` in BoundedBuffer.h

```cpp
shutdown_ = true;
cv_not_empty_.notify_all();
cv_not_full_.notify_all();
```

- Blocked **producer** wake — see shutdown, return without push
- Blocked **consumer** wake — see empty+shutdown, return nullopt

---

## Advantages

| Fayda | Detail |
|-------|--------|
| No lost jobs in buffer | Drain before exit |
| Clean thread join | Consumer loop ends predictably |
| Ops friendly | Deploy / restart safe |

**Galat:** `exit()` while items in queue — data loss.

---

## Run

```bash
./bin/05_graceful_shutdown
```

6 "processing" lines, "drained buffer, clean exit".

---

## Order matters

1. Finish producing (or stop accepting)
2. `signal_shutdown`
3. Consumers drain
4. `join` threads

---

## Interview

> "Shutdown flag plus notify_all lets blocked threads re-check and exit after draining."

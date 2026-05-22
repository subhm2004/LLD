# 04 — Graceful Shutdown

**Source:** `../04_graceful_shutdown.cpp` + `ThreadPool::shutdown()`  
**Binary:** `../bin/04_graceful_shutdown`

---

## Pattern context

Pool band karte waqt: pending tasks **complete**, threads **join**, koi detach zombie nahi.

---

## Is file mein kya ho raha hai

1. `pool(3)`, 5 tasks enqueue (300ms each inside task)
2. Main 400ms wait
3. **`pool.shutdown()`** explicit — destructor se pehle
4. `stop_=true`, `notify_all`, har worker join
5. Print `is_stopped()==true`, `worker_count()==0`

---

## `shutdown()` internals

```cpp
stop_ = true;
cv_.notify_all();
for (worker : workers) worker.join();
workers_.clear();
```

Workers: `stop && empty` → exit `worker_loop`.

---

## Advantages

| Fayda | Detail |
|-------|--------|
| No task loss | Queue drain before exit |
| Safe process exit | All threads joined |
| Deploy friendly | Rolling restart clean |

**Galat:** `exit(0)` abruptly — mid-task corrupt state.

---

## Run

```bash
./bin/04_graceful_shutdown
```

5 "complete" lines, phir stopped=true.

---

## Relation

Signaling `notify_all`: [`../../Signaling_Pattern/docs/05_shutdown_notify_all.md`](../../Signaling_Pattern/docs/05_shutdown_notify_all.md)

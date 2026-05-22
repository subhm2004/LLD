# 02 — Many Tasks Queued

**Source:** `../02_many_tasks_queued.cpp`  
**Binary:** `../bin/02_many_tasks_queued`

---

## Pattern context

Pool size **chhota**, tasks **zyada** — extra tasks **queue** mein wait. Throughput = `worker_count × task_duration`.

---

## Is file mein kya ho raha hai

- `ThreadPool pool(2)` — 2 workers
- 10 tasks enqueue — har baad `pending_tasks()` print
- Har task 200ms sleep inside lambda
- `shutdown()` end par (via destructor)

---

## Kya observe karna hai

- Enqueue fast — `pending` size badhta hai jab workers busy
- Tasks finish in rough FIFO order
- Total time ≈ `10 × 200ms / 2` ≈ 1 second + overhead (not 10×200 serial)

---

## Advantages

| Fayda | Detail |
|-------|--------|
| Smooth overload | Burst of requests queue, not 1000 threads |
| Fair FIFO default | First enqueued first run |
| Tunable capacity | 2 workers = max 2 parallel |

---

## Production extension

- **Bounded queue** — full hone par reject / block producer
- **Priority queue** — urgent tasks pehle

---

## Run

```bash
./bin/02_many_tasks_queued
```

---

## Interview

> "Thread pool limits parallelism; excess work waits in queue instead of spawning threads."

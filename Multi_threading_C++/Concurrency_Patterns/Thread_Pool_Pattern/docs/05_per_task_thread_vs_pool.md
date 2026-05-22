# 05 — Per-Task Thread vs Thread Pool

**Source:** `../05_per_task_thread_vs_pool.cpp`  
**Binary:** `../bin/05_per_task_thread_vs_pool`

---

## Pattern context

**Why pool exists** — quantitative intuition. Same 40 small jobs, do implementations, compare ms.

---

## Is file mein kya ho raha hai

### A) `demo_per_task_threads`

```cpp
for (i = 0; i < 40; ++i)
    threads.emplace_back(small_job, i);
for (t : threads) t.join();
```

40× create + 40× join overhead.

### B) `demo_thread_pool`

```cpp
ThreadPool pool(4);
for (i = 0; i < 40; ++i)
    pool.enqueue([i]{ small_job(i); });
pool.shutdown();
```

4 threads reuse, 40 tasks queued.

---

## Advantages of pool (this benchmark)

| Aspect | Per-thread | Pool |
|--------|------------|------|
| OS thread ops | 40 create | 4 create |
| Peak threads | 40 | 4 |
| Typical time | Often higher ms | Often lower ms |

*Numbers machine dependent — trend matters.*

---

## Run

```bash
./bin/05_per_task_thread_vs_pool
```

---

## When per-task thread still OK

- Rare long-running background job
- Task count very low (1–2)
- Isolation required per task (crash containment)

---

## Interview

> "Pool amortizes thread lifecycle cost and caps concurrency; per-task threads don't scale to thousands of short jobs."

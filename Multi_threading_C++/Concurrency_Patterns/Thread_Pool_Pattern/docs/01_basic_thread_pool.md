# 01 — Basic Thread Pool

**Source:** `../01_basic_thread_pool.cpp` + `../ThreadPool.h`  
**Binary:** `../bin/01_basic_thread_pool`

---

## Pattern context (Thread Pool)

Fixed workers + task queue. Is demo mein minimum path: create pool → enqueue lambdas → destructor shutdown.

---

## Is file mein kya ho raha hai

1. `ThreadPool pool(3)` — constructor 3× `worker_loop` threads start
2. Loop `i=1..6`: `pool.enqueue([i]{ ... sleep 150ms ... })`
3. Main `sleep(2)` — workers background mein tasks run
4. `main` return → `~ThreadPool()` → `shutdown()` → all `join`

---

## `ThreadPool.h` — constructor & enqueue

**Constructor:** har worker:

```text
forever:
  wait for task or stop
  pop task
  run task outside lock
```

**enqueue:**

```cpp
lock; if (!stop) tasks.push(task);
cv.notify_one();
```

---

## Advantages (is demo se clear)

| Fayda | Proof in output |
|-------|-----------------|
| Parallelism | 3 tasks ek saath start (different thread ids) |
| Reuse | Same 3 ids multiple tasks par |
| No per-task thread create | Sirf 6 enqueue, 3 threads |

---

## Run

```bash
./compile.sh && ./bin/01_basic_thread_pool
```

---

## Common mistakes

| Galati | Fix |
|--------|-----|
| Task run inside queue lock | Pop then unlock then `task()` |
| Bhool jana destructor join | Always shutdown in dtor |
| Pool size = unlimited tasks RAM | Production: bound queue |

---

## Next

[`02_many_tasks_queued.md`](./02_many_tasks_queued.md) — backlog

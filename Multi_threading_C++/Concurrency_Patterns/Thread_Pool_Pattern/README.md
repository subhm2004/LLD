# Thread Pool Pattern — Detailed Guide (C++17)

## 1. Pattern kya hai?

**Thread Pool** = startup par **fixed number** of worker threads + **task queue**.

Naya kaam aata hai → queue mein daalo → koi free worker uthake chalata hai → worker wapas wait loop mein.

| Har task par `new thread` | Thread Pool |
|---------------------------|-------------|
| Create + join har baar (slow) | Threads **ek baar** bante hain, **reuse** |
| 1000 tasks → 1000 threads risk | Max N threads — memory predictable |
| OS scheduler overload | Controlled parallelism |

**Analogy:** Har customer ke liye naya cashier hire mat karo — 4 permanent cashiers, token queue mein order.

---

## 2. Kyun use karte hain? (Advantages)

| Advantage | Detail |
|-----------|--------|
| **Performance** | Thread creation ~ms level costly — reuse se bachta hai |
| **Resource control** | RAM/CPU cap — `pool(4)` matlab max 4 parallel |
| **Ready workers** | Task aate hi kaam — cold start nahi |
| **Production standard** | Web servers, DB pools, game engines |

**Kab use karo:**

- Bahut saare chhote/medium tasks (HTTP handlers, file chunks, image tiles)
- Background job processing
- Async work without unbounded thread explosion

**Kab mat use karo / alternatives:**

- Sirf 1 lamba task — single thread kaafi
- Strict per-key ordering chahiye — dedicated queue per key
- CPU already 100% — zyada threads se slow (context switch)

---

## 3. Core components (`ThreadPool.h`)

| Component | File mein kya hai |
|-----------|-------------------|
| `vector<thread> workers_` | Fixed workers, har ek `worker_loop` |
| `queue<function<void()>> tasks_` | Pending kaam FIFO |
| `mutex queue_mtx_` | Queue thread-safe |
| `condition_variable cv_` | Worker sleep until task / shutdown |
| `atomic<bool> stop_` | Graceful band |

### Worker loop (important)

```text
wait until (tasks not empty OR stop)
if stop && empty → exit thread
pop task
unlock
run task()   ← LOCK KE BAHAR — zaroori!
```

**Kyun task lock ke bahar?**  
Lock andar rakho to ek waqt sirf ek thread kaam karegi — pool ka fayda khatam.

### `enqueue(task)`

1. Mutex lock → push task  
2. `cv_.notify_one()` — **ek** sleeping worker jagao

### `submit(f, args...)` (C++17)

- `packaged_task` + `shared_ptr` + `future` return  
- Result bina manual flag ke

### `shutdown()` / destructor

- `stop_=true` → `notify_all()` → har worker `join()`

Full header walkthrough: [`THREAD_POOL_PATTERN_COMPLETE.md`](./THREAD_POOL_PATTERN_COMPLETE.md)

---

## 4. Folder structure

```
Thread_Pool_Pattern/
├── ThreadPool.h              ← shared implementation
├── 01_basic_thread_pool.cpp
├── 02_many_tasks_queued.cpp
├── 03_thread_reuse.cpp
├── 04_graceful_shutdown.cpp
├── 05_per_task_thread_vs_pool.cpp
├── 06_submit_with_future.cpp
├── docs/
├── compile.sh / run_all.sh
└── bin/
```

---

## 5. Build & run

```bash
cd Thread_Pool_Pattern
./compile.sh
./bin/01_basic_thread_pool
./run_all.sh
```

---

## 6. Har demo — code mein kya ho raha hai

### 01 — `01_basic_thread_pool.cpp`

**Flow:**

- `ThreadPool pool(3)` — 3 workers start, wait loop mein
- 6 lambdas `enqueue` — workers parallel run
- `main` 2 sec sleep → return → destructor `shutdown()` + join

**Seekhne ko:**

- Basic API: `enqueue([...]{ })`
- Output mein 3 alag `thread::id` dikhengi

**Fayda:** Minimum working pool — baaki demos isi par build.

**Detail:** [`docs/01_basic_thread_pool.md`](./docs/01_basic_thread_pool.md)

---

### 02 — `02_many_tasks_queued.cpp`

**Flow:**

- 2 workers, 10 tasks
- Har enqueue par `pending_tasks()` print
- Tasks queue mein wait jab dono workers busy

**Seekhne ko:**

- Throughput = worker count limited
- FIFO processing

**Fayda:** Backlog samajh — production mein bounded queue + rejection bhi add hota hai.

**Detail:** [`docs/02_many_tasks_queued.md`](./docs/02_many_tasks_queued.md)

---

### 03 — `03_thread_reuse.cpp`

**Flow:**

- 12 tasks, 3 workers
- `thread::id` map karke worker#1, #2, #3 label
- Same id baar-baar print

**Seekhne ko:**

- Pool ka **asli benefit** — thread recreate nahi

**Fayda:** Interview proof: "reuse reduces OS overhead".

**Detail:** [`docs/03_thread_reuse.md`](./docs/03_thread_reuse.md)

---

### 04 — `04_graceful_shutdown.cpp`

**Flow:**

- 5 tasks enqueue
- Explicit `pool.shutdown()` before scope end
- Workers pending finish → exit

**Seekhne ko:**

- `notify_all` on shutdown
- `is_stopped()`, `worker_count()==0` after shutdown

**Fayda:** Server stop / executor drain — data loss nahi.

**Detail:** [`docs/04_graceful_shutdown.md`](./docs/04_graceful_shutdown.md)

---

### 05 — `05_per_task_thread_vs_pool.cpp`

**Flow:**

- 40 light CPU jobs
- Pehle: 40× `thread` create+join
- Phir: `ThreadPool(4)` + 40 enqueue

**Seekhne ko:**

- Timing compare — pool often faster (machine dependent)

**Fayda:** Quantitative intuition "why pool".

**Detail:** [`docs/05_per_task_thread_vs_pool.md`](./docs/05_per_task_thread_vs_pool.md)

---

### 06 — `06_submit_with_future.cpp`

**Flow:**

- `pool.submit(compute_square, i)` → `future<int>`
- Main parallel submit, baad mein `get()` se results

**Seekhne ko:**

- Async result without shared atomic flag
- `invoke_result_t` (C++17)

**Fayda:** Map-reduce style parallel compute.

**Pitfall:** Same pool worker par `submit` + `get()` nested → deadlock risk.

**Detail:** [`docs/06_submit_with_future.md`](./docs/06_submit_with_future.md)

---

## 7. Optimal thread count (practical)

| Work | Threads |
|------|---------|
| CPU-bound (math, encode) | `thread::hardware_concurrency()` |
| I/O-bound (network, disk) | cores × 2 ya zyada |

Formula (rough): `threads = cores × (1 + wait_time / compute_time)`

---

## 8. Thread pool vs Producer-Consumer

| Thread Pool | Producer-Consumer |
|-------------|-------------------|
| Focus: **worker reuse** | Focus: **buffer between stages** |
| Clients `enqueue` work | Producers `produce`, consumers `consume` |

Pool ki internal queue bhi producer-consumer jaisi hai — [`../Producer_Consumer_Pattern/`](../Producer_Consumer_Pattern/).

Internal signaling: [`../Signaling_Pattern/`](../Signaling_Pattern/).

---

## 9. Interview Q&A

| Sawal | Jawab |
|-------|-------|
| Pool vs new thread per task? | Amortized create cost, bounded concurrency |
| `notify_one` enqueue par? | Ek task → ek worker kaafi |
| Deadlock? | Worker task ke andar same pool par blocking `get()` |
| Unbounded queue risk? | Memory grow — production mein limit + reject |

---

## 10. Related code

- [`../../thread_pool.cpp`](../../thread_pool.cpp) — original Hinglish comments wala version

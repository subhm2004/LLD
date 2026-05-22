# 06 — Submit with Future

**Source:** `../06_submit_with_future.cpp` + `ThreadPool::submit()`  
**Binary:** `../bin/06_submit_with_future`

---

## Pattern context

`enqueue` = fire-and-forget. `submit` = **result chahiye** — `future` se async return value.

---

## Is file mein kya ho raha hai

1. `pool(3)`
2. `futures.push_back(pool.submit(compute_square, i))` for i=1..6
3. Main other work 100ms
4. Loop `futures[i].get()` — print `square(n)=n²`

---

## `submit` implementation (ThreadPool.h)

```cpp
packaged_task<Ret()> wrapped bind(f, args...);
future = task.get_future();
enqueue([task_ptr]{ (*task_ptr)(); });
return future;
```

`shared_ptr` — lambda task ke baad bhi packaged_task zinda.

---

## Advantages

| Fayda | Detail |
|-------|--------|
| Typed result | No void* / manual atomic |
| Parallel submit | All 6 queue quickly |
| Lazy get | Main chooses when to block |

---

## Pitfall — deadlock

Worker thread (pool ka hi) andar:

```cpp
auto f = pool.submit(...);
f.get();  // same pool full → deadlock
```

Fix: alag pool, `async`, or don't block worker on own pool.

---

## Run

```bash
./bin/06_submit_with_future
```

square(1)=1 … square(6)=36

---

## C++17

`std::invoke_result_t` — return type deduction for `submit`.

---

## Interview

> "submit wraps packaged_task in enqueue so pool runs work and future delivers result."

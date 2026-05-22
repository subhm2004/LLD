# 02 — Multiple Producers One Consumer (MPSC)

**Source:** `../02_multiple_producers_one_consumer.cpp`  
**Binary:** `../bin/02_multiple_producers_one_consumer`

---

## Pattern context

**Many writers, one reader** — logs, metrics, event aggregation.

---

## Is file mein kya ho raha hai

- `BoundedBuffer(4)` — small buffer stress
- `producer_fn(id)`: 5 items each, id*100+i format
- Threads `p1`, `p2` parallel push
- **Mutex** inside `produce()` — pushes serialized, safe
- One `consumer` drains until shutdown
- `p1.join(); p2.join();` then `signal_shutdown()`

---

## Thread safety kahan hai?

Har `produce()` / `consume()` apna `unique_lock` on same `mtx_` — race on `queue` impossible.

---

## Advantages

| Fayda | Real system |
|-------|-------------|
| Writers don't block each other long | Only mutex hold during push |
| Single consumer ordering | FIFO global order (mostly) |
| One flush point | Disk/network writer |

---

## Output pattern

Items interleave: `101, 201, 102...` — depends on scheduler.

`total consumed` ≈ 10.

---

## Run

```bash
./bin/02_multiple_producers_one_consumer
```

---

## Pitfall

Shutdown **before** join producers → consumer might exit early with items unproduced. Yahan producers pehle join, phir shutdown — correct order.

---

## Interview

> "MPSC is safe with mutex-protected buffer; consumer is single so no duplicate pops."

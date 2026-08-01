# 03 — One Producer Multiple Consumers (SPMC)

**Source:** `../03_one_producer_multiple_consumers.cpp`  
**Binary:** `../bin/03_one_producer_multiple_consumers`

---

## Pattern context

**One source, many workers** — job queue with competing consumers (thread pool consume side jaisa).

---

## Is file mein kya ho raha hai

- Producer: 12 ints, 50ms apart
- 3 consumers: `while (consume())` — **race** safe kyunki pop mutex ke andar
- `atomic total_consumed` — verify 12 at end
- Producer `signal_shutdown()` → all consumers exit

---

## Signaling per item

Har `produce` → `notify_one` — **ek** consumer wake (others sleep until next item).

---

## Advantages

| Fayda | Detail |
|-------|--------|
| Parallel processing | 3× throughput potential |
| Load spread | Idle consumer next item le |
| Same buffer | No per-consumer queues needed (simple design) |

---

## vs Thread Pool

| SPMC buffer | Thread pool |
|-------------|-------------|
| Items = data | Tasks = functions |
| Consumers pop | Workers pop `function` |

See [`../../Thread_Pool_Pattern/README.md`](../../Thread_Pool_Pattern/README.md)

---

## Run

```bash
./bin/03_one_producer_multiple_consumers
```

`[C1]`, `[C2]`, `[C3]` mixed; `total_consumed=12`.

---

## Pitfall

Consumer crash — design mein restart/supervisor chahiye production mein.

---

## Interview

> "notify_one wakes one consumer per item; mutex ensures exactly one pop."

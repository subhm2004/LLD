# 04 — Future / Promise Signaling

**Source:** `../04_future_promise_signal.cpp`  
**Binary:** `../bin/04_future_promise_signal`

---

## Pattern context

Kabhi signal sirf "ho gaya" nahi — **result bhi** chahiye. `promise`/`future` = one-shot signal + data transfer.

---

## Is file mein kya ho raha hai

1. `promise<int> prom`, `future<int> fut = prom.get_future()`
2. Worker thread: `heavy_computation(42)` → `prom.set_value(1764)`
3. Main: pehle 3 "other work" steps (300ms each)
4. Main: `fut.get()` — **block** until worker sets promise
5. Print result

---

## Signaling without CV

| Piece | Role |
|-------|------|
| `set_value` | Signal: result ready |
| `get()` | Wait for signal |
| Shared state | Inside `future` implementation (std lib) |

---

## Advantages

| Fayda | Detail |
|-------|--------|
| Type-safe result | `future<int>` |
| No manual flag+mutex | Less boilerplate for one-shot |
| Composable | `async`, thread pool `submit` |

---

## Pitfalls

- `set_value` twice → exception
- `promise` destroyed before set → broken_future
- Worker exception → use `set_exception`

---

## Run

```bash
./bin/04_future_promise_signal
```

~1s baad `result = 1764`.

---

## Interview

> "Future/promise is a standardized one-shot signaling channel for async results."

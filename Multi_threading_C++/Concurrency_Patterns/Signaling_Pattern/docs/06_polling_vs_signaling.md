# 06 — Polling vs Signaling Comparison

**Source:** `../06_polling_vs_signaling.cpp`  
**Binary:** `../bin/06_polling_vs_signaling`

---

## Pattern context

**Anti-pattern demo:** same "wait until ready" — pehle galat tarika (polling), phir sahi (CV).

---

## Is file mein kya ho raha hai

### Part A — Polling

```cpp
while (!ready.load(memory_order_acquire)) {
    ++poll_iterations;
    // cap at 5M for demo
}
```

- Worker 500ms baad `ready=true`
- Main spins millions of times — CPU burn

### Part B — Signaling

```cpp
cv.wait(lock, [&] { return done; });
```

- Same 500ms worker delay
- Main ~0 spins — OS sleep

---

## Advantages of signaling (side-by-side)

| Metric | Polling | Signaling |
|--------|---------|-----------|
| CPU while waiting | High | ~0 |
| Power / heat | Bad on servers | Good |
| Scalability | 1 waiter = 1 core waste | Many waiters efficient |

---

## Kab polling OK hai?

- **Very short** waits (nanoseconds), lock-free atomics, expert tuning
- **Not** for I/O, seconds, ms-level blocking

---

## Run

```bash
./bin/06_polling_vs_signaling
```

Output: polling iterations in millions; signaling elapsed ~500ms, negligible spins.

---

## Interview one-liner

> "Never block-wait with busy loops; use condition_variable or future for efficiency."

---

## Takeaway

Signaling pattern ka **business value** = correct + **cheap wait**. Polling sirf teaching ke liye dikhao kya **mat** karna.

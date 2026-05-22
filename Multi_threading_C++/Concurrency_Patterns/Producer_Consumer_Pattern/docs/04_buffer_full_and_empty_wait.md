# 04 — Buffer Full & Empty Wait (Backpressure)

**Source:** `../04_buffer_full_and_empty_wait.cpp`  
**Binary:** `../bin/04_buffer_full_and_empty_wait`

---

## Pattern context

**Backpressure** = slow consumer automatically slows fast producer — bounded buffer + `cv_not_full.wait`.

---

## Is file mein kya ho raha hai

- Capacity **3** — chhota buffer
- Producer: 8 items, 50ms between tries
- Consumer: **600ms late start** — producer pehle buffer fill karega
- Producer prints `trying to produce X` — agar full ho to **stuck** until consumer pops

---

## Timeline (conceptual)

```
0ms    producer fills 1,2,3
150ms  producer wants 4 — BLOCKED (full)
600ms  consumer starts, pops 1
       producer wakes, produces 4...
```

---

## Advantages

| Fayda | System stability |
|-------|------------------|
| No OOM | Memory cap = capacity × item size |
| Natural throttle | Fast stage can't overwhelm slow |
| Visible in logs | "trying" then pause teaches debugging |

---

## Signaling involved

- Full → producer on `cv_not_full`
- Pop → `notify_one` on `cv_not_full` → producer wake

---

## Run

```bash
./bin/04_buffer_full_and_empty_wait
```

Clearly dekho producer block jab `size=3/3`.

---

## Production

Capacity tune karo: chhota = more backpressure, bada = more buffering latency.

---

## Interview

> "Bounded buffer with condition variables implements backpressure without dropping data."

# 01 — Single Producer Single Consumer (SPSC)

**Source:** `../01_single_producer_single_consumer.cpp` + `BoundedBuffer.h`  
**Binary:** `../bin/01_single_producer_single_consumer`

---

## Pattern context (Producer-Consumer)

Sabse simple form: **1 producer**, **1 consumer**, **bounded buffer**. Baaki saare variants isi par extra threads add karte hain.

---

## Is file mein kya ho raha hai

| Phase | Thread | Action |
|-------|--------|--------|
| Setup | main | `BoundedBuffer<int>(5)` |
| Produce | producer | `produce(1..10)` + 100ms sleep |
| Consume | consumer | `while (consume())` process + 150ms sleep |
| End | producer | `signal_shutdown()` |
| Exit | consumer | `nullopt` → loop break |

---

## `BoundedBuffer` calls

**produce:**

1. Wait until `size < capacity` (or shutdown)
2. Push item
3. `cv_not_empty.notify_one()` — consumer ko signal

**consume:**

1. Wait until `!empty` or shutdown
2. If empty after wake → `nullopt`
3. Pop, `cv_not_full.notify_one()` — producer ko signal

---

## Advantages (SPSC)

| Fayda | Detail |
|-------|--------|
| Simplest correct model | Interview drawing easy |
| Rate decoupling | Producer 100ms, consumer 150ms — buffer smooth karta |
| Memory bounded | Max 5 items in RAM |

---

## Output samjho

Kabhi `buffer size=2,3` dikhega — consumer slow → producer buffer bharta hai, phir producer **wait** (signaling).

---

## Run

```bash
./compile.sh && ./bin/01_single_producer_single_consumer
```

10 produced, 10 consumed, clean exit.

---

## Common mistakes

| Galati | Fix |
|--------|-----|
| Consumer spin on empty | `consume()` must wait inside buffer |
| Forget shutdown | Consumer infinite wait |
| Unbounded queue | Always set capacity |

---

## Next

[`02_multiple_producers_one_consumer.md`](./02_multiple_producers_one_consumer.md)

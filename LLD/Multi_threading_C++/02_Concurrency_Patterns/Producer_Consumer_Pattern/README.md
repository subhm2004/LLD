# Producer-Consumer Pattern — Detailed Guide (C++17)

## 1. Pattern kya hai?

Do (ya zyada) threads **alag speed** par kaam karte hain — beech mein **shared bounded buffer** (queue).

| Role | Kaam |
|------|------|
| **Producer** | Item banata / generate karta → buffer mein **push** |
| **Consumer** | Buffer se **pop** → process karta hai |
| **Bounded buffer** | Max size fixed — full/empty par **wait** (signaling) |

**Analogy:** Packing station — ek line box bharti hai, doosri line ship karti hai; table par zyada se zyada 5 box hi rakh sakte ho.

---

## 2. Kyun use karte hain? (Advantages)

| Advantage | Explanation |
|-----------|-------------|
| **Decoupling** | Producer consumer ki speed se independent |
| **Smooth bursts** | Fast producer slow consumer ko buffer se absorb |
| **Backpressure** | Buffer full → producer auto slow (system stable) |
| **Parallel pipeline** | Multiple stages production systems mein |

**Real use cases:**

- Print spooler (docs produce, printer consume)
- Message queues (Kafka-style pipelines)
- Log buffers (threads produce logs, one writer flushes)
- Video pipeline (capture → encode → stream stages)

**Kab avoid:**

- Single thread enough
- Strict real-time jahan bounded delay unacceptable without design
- Priority ordering — simple FIFO queue kaafi nahi

---

## 3. Teen conditions (har implementation mein)

### 1) Buffer FULL

Producer **ruk** jata hai:

```cpp
cv_not_full.wait(lock, [] { return size < capacity; });
push(item);
cv_not_empty.notify_one();
```

### 2) Buffer EMPTY

Consumer **ruk** jata hai:

```cpp
cv_not_empty.wait(lock, [] { return !empty; });
pop(item);
cv_not_full.notify_one();
```

### 3) Production khatam (shutdown)

```cpp
shutdown = true;
cv_not_empty.notify_all();
// consumer: empty && shutdown → exit
```

Yeh teeno **Signaling Pattern** use karte hain — [`../Signaling_Pattern/`](../Signaling_Pattern/).

---

## 4. `BoundedBuffer.h` — shared class

| Method | Kya karta hai |
|--------|----------------|
| `produce(T)` | Full ho to wait → push → consumer ko signal |
| `consume()` | Empty ho to wait → pop → `optional<T>` return |
| `signal_shutdown()` | Producers/consumers ko clean exit |
| `size()` / `capacity()` | Debug / monitoring |

C++17: `std::optional` — empty + shutdown par `nullopt`.

---

## 5. Folder structure

```
Producer_Consumer_Pattern/
├── BoundedBuffer.h
├── 01_single_producer_single_consumer.cpp
├── 02_multiple_producers_one_consumer.cpp
├── 03_one_producer_multiple_consumers.cpp
├── 04_buffer_full_and_empty_wait.cpp
├── 05_graceful_shutdown.cpp
├── 06_manual_dual_cv.cpp
├── docs/
├── compile.sh / run_all.sh
└── bin/
```

---

## 6. Build & run

```bash
cd Producer_Consumer_Pattern
./compile.sh
./bin/01_single_producer_single_consumer
./run_all.sh
```

---

## 7. Har demo — kya ho raha hai

### 01 — `01_single_producer_single_consumer.cpp` (SPSC)

**Flow:**

- `BoundedBuffer<int>(5)`
- Producer 1..10 `produce`, 100ms gap
- Consumer `consume()` loop until `nullopt`
- Producer end par `signal_shutdown()`

**Code samjho:**

```cpp
while (auto item = buffer.consume()) {
    // process *item
}
```

**Fayda:** Simplest correct model — interview start yahi se.

**Detail:** [`docs/01_single_producer_single_consumer.md`](./docs/01_single_producer_single_consumer.md)

---

### 02 — `02_multiple_producers_one_consumer.cpp` (MPSC)

**Flow:**

- 2 producer threads, same buffer
- Mutex har `produce()` ko serialize karta hai — safe
- 1 consumer, ~10 items total
- Shutdown after both `join`

**Fayda:** Log aggregation, multiple writers → one disk flush.

**Detail:** [`docs/02_multiple_producers_one_consumer.md`](./docs/02_multiple_producers_one_consumer.md)

---

### 03 — `03_one_producer_multiple_consumers.cpp` (SPMC)

**Flow:**

- 1 producer, 12 items
- 3 consumers compete — `notify_one` har item par ek consumer
- `atomic total_consumed` — end mein 12 verify

**Fayda:** Job queue + worker team — thread pool jaisa consume side.

**Detail:** [`docs/03_one_producer_multiple_consumers.md`](./docs/03_one_producer_multiple_consumers.md)

---

### 04 — `04_buffer_full_and_empty_wait.cpp`

**Flow:**

- Capacity **3** only
- Consumer **600ms late** start
- Fast producer → buffer full → **"trying to produce"** ke baad block dikhega

**Fayda:** Backpressure live demo — slow consumer fast producer ko rokta hai.

**Detail:** [`docs/04_buffer_full_and_empty_wait.md`](./docs/04_buffer_full_and_empty_wait.md)

---

### 05 — `05_graceful_shutdown.cpp`

**Flow:**

- `BoundedBuffer<string>` — 6 jobs
- `signal_shutdown()` — no more produce
- Consumer drain → clean exit

**Fayda:** Pipeline stop without losing buffered work.

**Detail:** [`docs/05_graceful_shutdown.md`](./docs/05_graceful_shutdown.md)

---

### 06 — `06_manual_dual_cv.cpp`

**Flow:**

- **Bina class** — global queue + `cv_not_full` + `cv_not_empty` + `done`
- Interview whiteboard jaisa code
- Same logic as `BoundedBuffer.h`

**Fayda:** Company interview mein class likhne ko na bole to bhi likh sakte ho.

**Detail:** [`docs/06_manual_dual_cv.md`](./docs/06_manual_dual_cv.md)

---

## 8. Variants table

| Variant | File | Meaning |
|---------|------|---------|
| SPSC | 01 | 1 producer, 1 consumer |
| MPSC | 02 | Many producers, one consumer |
| SPMC | 03 | One producer, many consumers |
| MPMC | (extend yourself) | Many both — mutex still OK |

---

## 9. vs Thread Pool

| Producer-Consumer | Thread Pool |
|-------------------|-------------|
| Data **buffer** between stages | **Workers** reuse for tasks |
| Rate matching problem | Parallelism control |

Thread pool task queue = consumer side of producer-consumer.

---

## 10. Common mistakes

| Galati | Fix |
|--------|-----|
| Sirf ek `condition_variable` | Do alag: not_full, not_empty |
| `pop` without wait on empty | Consumer must wait |
| `notify` before state update | Mutex ke andar state, phir notify |
| Busy-wait on empty/full | Always `cv.wait` + predicate |

---

## 11. Interview Q&A

| Sawal | Jawab |
|-------|-------|
| Kyun 2 CV? | Alag wait conditions — full vs empty |
| `notify_one` vs `notify_all`? | Normal ops = one; shutdown = all |
| Lost wakeup? | Predicate + mutex discipline |
| Bounded vs unbounded? | Bounded = backpressure + memory cap |

---

## 12. Extended theory

[`PRODUCER_CONSUMER_PATTERN_COMPLETE.md`](./PRODUCER_CONSUMER_PATTERN_COMPLETE.md)

## 13. Related

- [`../../05_Classic_Problems/Producer_Consumer_Legacy/producer_consumer.cpp`](../../05_Classic_Problems/Producer_Consumer_Legacy/producer_consumer.cpp)
- [`../Signaling_Pattern/02_producer_consumer_signal.cpp`](../Signaling_Pattern/02_producer_consumer_signal.cpp)

# Signaling Pattern — Detailed Guide (C++17)

## 1. Pattern kya hai?

**Signaling** = ek thread dusri thread ko **batati hai** ki ab aage badh sakti ho — koi shared condition true ho gayi hai.

| Bina signaling (polling) | Signaling (`condition_variable`) |
|------------------------|----------------------------------|
| `while (!ready) { }` — CPU burn | Thread **sleep** jab tak notify na ho |
| Busy-wait, cache thrashing | OS scheduler efficient wake-up |
| Latency unpredictable | Event-driven wake |

**Real-life analogy:** Delivery boy har 10 sec door knock karta hai (polling) vs tum phone karke "aa jao" bolte ho (signal).

---

## 2. Kab use karte hain? (Advantages)

| Advantage | Explanation |
|-----------|-------------|
| **CPU bachata hai** | Wait karne wali thread idle — spin nahi |
| **Correct synchronization** | "Data ready" / "slot free" jaisi conditions safe |
| **Decoupling** | Producer/consumer timing alag ho sakti hai |
| **Standard C++11+** | `std::condition_variable` portable |

**Kab zaroori hai:**

- Background init complete hone ka wait (DB connect, config load)
- Producer-consumer: buffer mein item aaya / jagah khali
- Thread pool: naya task queue mein aaya
- Graceful shutdown: `done=true` broadcast

**Kab avoid / careful:**

- Microsecond-level latency jahan spin + atomic OK ho (rare, expert code)
- `notify` bina mutex discipline → lost wakeup bugs

---

## 3. Core building blocks (is repo mein)

| Piece | Role |
|-------|------|
| `std::mutex` | Shared flag / queue protect |
| `std::condition_variable` | `wait`, `notify_one`, `notify_all` |
| `std::unique_lock` | `wait()` ke liye required |
| **Predicate** | `wait(lock, []{ return cond; })` — spurious wakeup safe |
| `std::future` / `promise` | One-shot "result ready" signal |

---

## 4. Folder structure

```
Signaling_Pattern/
├── 01_condition_variable_basics.cpp
├── 02_producer_consumer_signal.cpp
├── 03_semaphore_signaling.cpp
├── 04_future_promise_signal.cpp
├── 05_shutdown_notify_all.cpp
├── 06_polling_vs_signaling.cpp
├── docs/          ← har .cpp ki alag deep README
├── compile.sh
├── run_all.sh
└── bin/           ← executables (gitignored)
```

---

## 5. Build & run

```bash
cd Signaling_Pattern
./compile.sh
./bin/01_condition_variable_basics
./run_all.sh    # saari 6 demos
```

Compiler: **C++17**, `-pthread`

---

## 6. Har demo — kya ho raha hai, kyun, fayda

### 01 — `01_condition_variable_basics.cpp`

**Kya ho raha hai:**

1. `main` worker thread start karta hai
2. `main` `cv.wait()` par block — jab tak `initialized == false`
3. Worker 2 sec "load" simulate karta hai
4. Worker mutex lock → `initialized = true` → `cv.notify_one()`
5. `main` wake → "received signal" print → `join`

**Is code mein important lines:**

- `cv.wait(lock, [] { return initialized; })` — predicate ke bina spurious wakeup se bug
- `notify_one()` **mutex ke baad** — state pehle set, phir signal

**Fayda:** Server start mein main thread block nahi busy-wait karti — worker ready hone par hi aage.

**Detail:** [`docs/01_condition_variable_basics.md`](./docs/01_condition_variable_basics.md)

---

### 02 — `02_producer_consumer_signal.cpp`

**Kya ho raha hai:**

- Shared `queue<int>` capacity 5
- **2 condition variables:**
  - `cv_not_full` — producer wait jab buffer full
  - `cv_not_empty` — consumer wait jab buffer empty
- 2 producers × 8 items, 1 consumer
- End: `producers_done=true` + `notify_all` → consumer exit

**Signaling flow:**

```
Producer push → notify cv_not_empty (consumer jagao)
Consumer pop  → notify cv_not_full  (producer jagao)
```

**Fayda:** Producer-consumer ka heart **signaling** hai — polling se buffer check karna galat.

**Detail:** [`docs/02_producer_consumer_signal.md`](./docs/02_producer_consumer_signal.md)

---

### 03 — `03_semaphore_signaling.cpp`

**Kya ho raha hai:**

- Custom `CountingSemaphore` (C++17 — internally `cv` + `permits_`)
- Pool size 3 = max 3 concurrent "connections"
- 5 workers `acquire()` → 2 block until `release()`

**Signaling angle:**

- `release()` → `notify_one()` = "ek permit free hai"
- `acquire()` → `wait` until `permits_ > 0`

**Fayda:** Resource counting + sleep until slot — same idea as CV, different API.

**Detail:** [`docs/03_semaphore_signaling.md`](./docs/03_semaphore_signaling.md)

---

### 04 — `04_future_promise_signal.cpp`

**Kya ho raha hai:**

- `promise<int>` worker set karta hai result
- `main` `future.get()` par block — async kaam khatam hone ka **one-shot signal**
- Beech mein main "other work" karta hai

**Fayda:** Callback hell ke bina async result; thread-safe handoff.

**Detail:** [`docs/04_future_promise_signal.md`](./docs/04_future_promise_signal.md)

---

### 05 — `05_shutdown_notify_all.cpp`

**Kya ho raha hai:**

- `TaskQueue`: workers `cv.wait` jab queue empty
- `request_shutdown()` → `shutdown=true` + **`notify_all`**
- Har worker wake → empty+shutdown → exit

**`notify_one` vs `notify_all` yahan:**

- Shutdown par **sab** workers ko jagana zaroori — warna kuch sleep mein reh jaayengi

**Fayda:** Thread pool / worker pool band karte waqt clean exit.

**Detail:** [`docs/05_shutdown_notify_all.md`](./docs/05_shutdown_notify_all.md)

---

### 06 — `06_polling_vs_signaling.cpp`

**Kya ho raha hai:**

- Same delay (~500ms) do tarike se wait:
  1. **Polling:** `while (!ready)` — millions of iterations
  2. **Signaling:** `cv.wait` — ~0 CPU spin

**Fayda:** Anti-pattern demo — interview mein "polling bad for blocking wait" clear bol sakte ho.

**Detail:** [`docs/06_polling_vs_signaling.md`](./docs/06_polling_vs_signaling.md)

---

## 7. Learning order

```
01 → 02 → 03 → 04 → 05 → 06
(basics → dual CV → semaphore → future → shutdown → polling compare)
```

---

## 8. Doosre patterns se relation

| Pattern | Signaling use |
|---------|----------------|
| [Producer-Consumer](../Producer_Consumer_Pattern/) | `cv_not_full` / `cv_not_empty` |
| [Thread Pool](../Thread_Pool_Pattern/) | `enqueue` → `notify_one` |

---

## 9. Interview — quick answers

| Sawal | Jawab |
|-------|-------|
| Spurious wakeup? | OS kabhi bina reason wake kar deta hai → predicate/while loop |
| `notify_one` vs `notify_all`? | Ek waiter vs sab; shutdown = all |
| Lost wakeup? | State mutex ke andar change, phir notify; wait mein predicate check |
| Mutex bina `wait`? | Undefined behavior risk |

---

## 10. Extended theory

Aur diagrams + API tables: [`SIGNALING_PATTERN_COMPLETE.md`](./SIGNALING_PATTERN_COMPLETE.md)

## 11. Related (parent folder)

- [`../../01_Fundamentals/lesson_3.cpp`](../../01_Fundamentals/lesson_3.cpp) — turn-based CV
- [`../../05_Classic_Problems/Producer_Consumer_Legacy/producer_consumer.cpp`](../../05_Classic_Problems/Producer_Consumer_Legacy/producer_consumer.cpp)
- [`../../01_Fundamentals/semaphor.cpp`](../../01_Fundamentals/semaphor.cpp)

# 02 — Producer-Consumer via Dual Signaling

**Source:** `../02_producer_consumer_signal.cpp`  
**Binary:** `../bin/02_producer_consumer_signal`

---

## Pattern context

Producer-Consumer **asli pattern** alag folder mein hai — yeh file dikhati hai ki uske andar **sirf signaling** ka kaam hai: 2 condition variables.

---

## Is file mein kya ho raha hai

- `queue<int> buffer`, capacity 5
- **Producer:** `cv_not_full.wait` until `size < 5` → push → `cv_not_empty.notify_one`
- **Consumer:** `cv_not_empty.wait` until `!empty || producers_done` → pop → `cv_not_full.notify_one`
- 2 producers (8 items each), 1 consumer
- End: `producers_done=true` + `notify_all` → consumer break

---

## Signaling table

| Event | Who waits | CV | Who notifies |
|-------|-----------|-----|--------------|
| Buffer full | Producer | `cv_not_full` | Consumer after pop |
| Buffer empty | Consumer | `cv_not_empty` | Producer after push |
| All producers done | Consumer | `cv_not_empty` | main `notify_all` |

---

## Advantages

| Fayda | Detail |
|-------|--------|
| No busy-wait on buffer | CPU efficient |
| Backpressure | Full buffer blocks producer automatically |
| Clean shutdown | `done` flag + broadcast |

---

## Code walkthrough (producer)

```cpp
cv_not_full.wait(lock, [] { return buffer.size() < kCapacity; });
buffer.push(item);
lock.unlock();
cv_not_empty.notify_one();
```

`unlock` ke baad notify — standard; state already consistent under lock before unlock.

---

## Run

```bash
./bin/02_producer_consumer_signal
```

Interleaved `[P1]`, `[P2]`, `[C1]` lines with `size=`.

---

## Relation

Full producer-consumer module: [`../../Producer_Consumer_Pattern/README.md`](../../Producer_Consumer_Pattern/README.md)

---

## Interview

> "Two condition variables because 'not full' and 'not empty' are different wait conditions."

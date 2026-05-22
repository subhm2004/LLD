# 06 — Manual Dual Condition Variable (Interview Code)

**Source:** `../06_manual_dual_cv.cpp`  
**Binary:** `../bin/06_manual_dual_cv`

---

## Pattern context

Companies kabhi **class nahi** maangte — sirf global queue + 2 CV. Yeh file `BoundedBuffer.h` ka **raw equivalent** hai.

---

## Is file mein kya ho raha hai

**Globals:**

- `queue<int> buffer`, `kCapacity=4`
- `cv_not_full`, `cv_not_empty`, `done`

**producer(id):**

```cpp
wait until size < kCapacity
push item
notify cv_not_empty
```

**consumer:**

```cpp
wait until !empty || done
if empty && done → break
pop
notify cv_not_full
```

**main:** producer join → `done=true` → `notify_all` → consumer join

---

## Mapping: manual ↔ BoundedBuffer

| Manual | Class method |
|--------|--------------|
| `cv_not_full.wait` | `produce` wait |
| `cv_not_empty.wait` | `consume` wait |
| `done` | `shutdown_` |

---

## Advantages of learning manual form

| Fayda | Detail |
|-------|--------|
| Whiteboard ready | No template syntax |
| Interview confidence | Explain every line |
| Debug skills | GDB mein wait queues samajh |

Production mein class/template use karo — reuse + type safety.

---

## Run

```bash
./bin/06_manual_dual_cv
```

Compare with [`../../05_Classic_Problems/Producer_Consumer_Legacy/producer_consumer.cpp`](../../05_Classic_Problems/Producer_Consumer_Legacy/producer_consumer.cpp) — repo original.

---

## Common interview follow-ups

1. Kyun 2 CV? — Different conditions
2. `notify_one` vs `notify_all`? — Normal vs shutdown
3. Predicate in wait? — Spurious wakeup

---

## Next step for you

`BoundedBuffer.h` khol ke line-by-line map karo manual code se — ek baar khud likhoge to yaad rahega.

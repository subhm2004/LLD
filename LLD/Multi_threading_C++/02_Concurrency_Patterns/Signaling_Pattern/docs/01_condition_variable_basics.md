# 01 — Condition Variable Signaling Basics

**Source:** `../01_condition_variable_basics.cpp`  
**Binary:** `../bin/01_condition_variable_basics`

---

## Pattern context (Signaling)

Ek thread (**worker**) kaam karti hai; **main** tab tak wait kare jab tak worker "ready" signal na de. Yeh **init/sync** ka classic case hai.

---

## Is file mein kya ho raha hai (step-by-step)

| Step | Thread | Action |
|------|--------|--------|
| 1 | main | `thread t(worker)` start |
| 2 | main | `cv.wait(..., initialized)` — **sleep** |
| 3 | worker | 2 sec simulate load |
| 4 | worker | `lock` → `initialized=true` → `unlock` → `notify_one()` |
| 5 | main | Wake, predicate true, print "received signal" |
| 6 | main | `t.join()` |

---

## Important code blocks

**Wait (main):**

```cpp
unique_lock<mutex> lock(mtx);
cv.wait(lock, [] { return initialized; });
```

- Predicate `return initialized` — spurious wakeup par dubara check
- `unique_lock` — `wait` internally unlock/lock karta hai

**Signal (worker):**

```cpp
lock_guard<mutex> lock(mtx);
initialized = true;
cv.notify_one();
```

- State **pehle** mutex ke andar, phir notify — lost wakeup avoid

---

## Advantages (yeh approach kyun)

| Fayda | Detail |
|-------|--------|
| Zero CPU spin | Main thread OS par sleep |
| Clear handoff | "Init done" explicit flag |
| Scalable pattern | Same for DB ready, cache warm, etc. |

**Bina iske:** `while (!initialized);` — 1 core 100% busy.

---

## `notify_one` yahan kyun?

Sirf **ek** waiter (main) hai → `notify_one` kaafi. Zyada waiters ho to strategy badlegi.

---

## Run & expected output

```bash
./compile.sh && ./bin/01_condition_variable_basics
```

~2 sec wait, phir `[main] received signal`.

---

## Common mistakes

| Galati | Fix |
|--------|-----|
| `wait` bina predicate | `while (!init) cv.wait` ya predicate lambda |
| `initialized=true` bina lock | Data race |
| `notify` pehle, state baad | Lost wakeup risk |

---

## Interview one-liner

> "Main blocks on condition_variable until worker sets predicate under mutex and notifies."

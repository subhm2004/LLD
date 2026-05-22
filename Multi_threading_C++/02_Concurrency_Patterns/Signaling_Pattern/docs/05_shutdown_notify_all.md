# 05 — Shutdown with notify_all

**Source:** `../05_shutdown_notify_all.cpp`  
**Binary:** `../bin/05_shutdown_notify_all`

---

## Pattern context

Multiple threads **same CV** par wait kar rahi hain. Shutdown par **sab** ko jagana padta hai — `notify_all`.

---

## Is file mein kya ho raha hai

- `TaskQueue`: `queue<int>`, `shutdown` flag
- 3 worker threads: `pop()` → wait until task ya shutdown
- Main: 6 tasks push
- `request_shutdown()`: `shutdown=true` + `cv.notify_all()`
- Har worker: empty + shutdown → exit loop

---

## notify_one vs notify_all (yahan)

| API | Kab |
|-----|-----|
| `notify_one` | Naya task aaya — ek worker kaafi |
| `notify_all` | Shutdown — **har** waiter check kare exit |

Agar shutdown par sirf `notify_one` → 2 workers hamesha sleep (bug).

---

## Advantages

| Fayda | Detail |
|-------|--------|
| Clean pool stop | No zombie threads |
| Pending work done | Tasks drain before exit |
| Predictable lifecycle | Server deploy/restart |

---

## Code (`request_shutdown`)

```cpp
shutdown = true;
cv.notify_all();
```

Workers wake, see empty+shutdown, return false from `pop`.

---

## Run

```bash
./bin/05_shutdown_notify_all
```

3 workers process tasks, phir "exiting (shutdown signaled)".

---

## Relation

Thread pool shutdown: [`../../Thread_Pool_Pattern/docs/04_graceful_shutdown.md`](../../Thread_Pool_Pattern/docs/04_graceful_shutdown.md)

---

## Interview

> "Use notify_all when all waiters must re-check global state like shutdown."

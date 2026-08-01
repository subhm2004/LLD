# 03 — Thread Reuse Proof

**Source:** `../03_thread_reuse.cpp`  
**Binary:** `../bin/03_thread_reuse`

---

## Pattern context

Thread pool ka **core economic argument**: OS threads **recreate nahi** hote har task par.

---

## Is file mein kya ho raha hai

- `ThreadPool pool(3)`
- 12 tasks enqueue
- Har task apna `this_thread::get_id()` print
- Map: pehli baar id dikhe → worker#1, #2, #3 assign
- End: `Unique worker threads used: 3`

---

## Expected output pattern

```
[task 1] worker#1 thread_id=0x...
[task 2] worker#2 ...
[task 4] worker#1 ...   ← SAME id as task 1
```

---

## Advantages

| Fayda | Technical reason |
|-------|------------------|
| Faster task start | No clone()/thread create syscall |
| Less memory | Fixed thread stacks |
| Better locality | Same core cache warm |

**Bina pool:** 12 tasks → 12 create + 12 join.

---

## Run

```bash
./bin/03_thread_reuse
```

---

## Interview

> "I verify reuse by logging thread::id across tasks — count stays at pool size."

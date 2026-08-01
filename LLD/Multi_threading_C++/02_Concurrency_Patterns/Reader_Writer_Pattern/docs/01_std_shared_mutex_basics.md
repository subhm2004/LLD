# 01 — std::shared_mutex Basics

**Source:** `../01_std_shared_mutex_basics.cpp`  
**Binary:** `../bin/01_std_shared_mutex_basics`

---

## Pattern context

C++17 ka **official** reader-writer lock — production code mein prefer karo jab custom policy na chahiye.

---

## Code mein kya ho raha hai

| Thread | Lock | Action |
|--------|------|--------|
| reader 1,2 | `shared_lock<shared_mutex>` | Read `shared_value` 3× |
| writer 1 | `unique_lock<shared_mutex>` | `++shared_value` 2× exclusive |

`shared_lock` = multiple threads ek saath hold kar sakti hain **agar koi unique_lock na ho**.

---

## Advantages

| Fayda | Detail |
|-------|--------|
| Standard, tested | libc++ / libstdc++ maintained |
| Clear API | shared vs unique intent obvious |
| Read parallelism | 2 readers overlap in time |

---

## Run

```bash
./compile.sh && ./bin/01_std_shared_mutex_basics
```

Final `value` = 2 (two writer increments).

---

## Interview

> "Readers use shared_lock; writers use unique_lock on the same shared_mutex."

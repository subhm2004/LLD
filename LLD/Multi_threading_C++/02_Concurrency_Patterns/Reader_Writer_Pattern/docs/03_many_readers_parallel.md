# 03 — Many Readers Parallel

**Source:** `../03_many_readers_parallel.cpp`  
**Binary:** `../bin/03_many_readers_parallel`

---

## Kya prove karta hai

`max_concurrent_reads > 1` → multiple `shared_lock` **same waqt** active.

---

## Code idea

```cpp
shared_lock<shared_mutex> lock(mtx);
++concurrent_reads;  // inside shared section
// sleep 50ms
--concurrent_reads;
```

5 threads × 4 iterations — peak concurrency print.

---

## Advantages

| Fayda | Detail |
|-------|--------|
| Throughput | Read-heavy workload scale |
| No writer | Pure read benchmark |

---

## Run

```bash
./bin/03_many_readers_parallel
```

Check line: `Max concurrent reads observed: N` where N≥2.

---

## Note

Yeh demo **writer-free** hai — sirf read parallelism dikhane ke liye.

# 05 — Fix: scoped_lock (C++17)

**Source:** `../05_fix_scoped_lock.cpp`  
**Binary:** `../bin/05_fix_scoped_lock`

## Recommended

```cpp
scoped_lock lock(mtxA, mtxB);
```

Ek line — RAII — internally `std::lock` jaisa.

## Run

```bash
./bin/05_fix_scoped_lock
```

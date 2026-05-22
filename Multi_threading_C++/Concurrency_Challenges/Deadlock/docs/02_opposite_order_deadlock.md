# 02 — Opposite Order Deadlock

**Source:** `../02_opposite_order_deadlock.cpp`  
**Binary:** `../bin/02_opposite_order_deadlock`

## Kya hota hai

- T1: `lock(A)` → wait `B`  
- T2: `lock(B)` → wait `A`  
- `timed_mutex` + `try_lock_for(200ms)` — program **exit** karta hai, hang nahi

## Output meaning

`BLOCKED on mtxB` / `BLOCKED on mtxA` = circular wait formed.

## Real infinite hang

See `07_optional_infinite_deadlock.cpp` (manual).

## Run

```bash
./bin/02_opposite_order_deadlock
```

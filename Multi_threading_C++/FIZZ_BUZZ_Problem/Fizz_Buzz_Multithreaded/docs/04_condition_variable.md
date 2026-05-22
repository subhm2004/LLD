# 04 — Condition Variable (Recommended)

```cpp
cv.wait(lock, [&] { return current > n || turnFor(current) == expected; });
```

- **Predicate** avoids spurious wakeups printing out of order.
- **`notify_all`** wakes all waiters; only the matching role proceeds.

```bash
./bin/04_condition_variable
```

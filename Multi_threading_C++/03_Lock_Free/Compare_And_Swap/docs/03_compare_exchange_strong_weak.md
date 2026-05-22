# 03 — Strong vs Weak compare_exchange

**Source:** `../03_compare_exchange_strong_weak.cpp`

| API | Spurious failure? | Typical use |
|-----|-------------------|-------------|
| `weak` | Yes | Retry loops, lock-free structures |
| `strong` | No | One-shot conditional update |

## Next

[`04_cas_spin_lock.md`](../04_cas_spin_lock.md)

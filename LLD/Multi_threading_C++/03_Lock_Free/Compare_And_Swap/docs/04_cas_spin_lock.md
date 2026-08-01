# 04 — CAS Spin Lock

**Source:** `../04_cas_spin_lock.cpp` + `../SpinLock.h`

`compare_exchange_weak` on `atomic<bool>`: `false → true` (lock), `true → false` (unlock).

8 threads × 50k increments — verify `shared == 400000`.

## Next

[`05_aba_problem_and_version_stamp.md`](../05_aba_problem_and_version_stamp.md)

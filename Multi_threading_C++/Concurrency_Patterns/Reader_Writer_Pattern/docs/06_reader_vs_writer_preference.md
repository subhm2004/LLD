# 06 — Reader vs Writer Preference

**Source:** `../06_reader_vs_writer_preference.cpp` + `ReadWriteLock.h`  
**Binary:** `../bin/06_reader_vs_writer_preference`

---

## Do policies

### `ReadWriteLock` (reader-preference)

- Writer wait jab tak `readers_ > 0`
- Naye readers writer ke wait ke baad bhi aa sakte → **writer starvation** possible

### `WriterPreferenceRWLock`

- `writer_waiting_` true → naye readers block
- Writer ko turn jaldi milta hai

---

## Advantages trade-off

| Policy | Choose when |
|--------|-------------|
| Reader pref | Read latency critical (analytics) |
| Writer pref | Updates must land timely (status flags) |

---

## Run

```bash
./bin/06_reader_vs_writer_preference
```

Dono sections print — compare log timing mentally.

---

## Interview

> "Reader-writer locks need a fairness policy; default std::shared_mutex implementation is implementation-defined."

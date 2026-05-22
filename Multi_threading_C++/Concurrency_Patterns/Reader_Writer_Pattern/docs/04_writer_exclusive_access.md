# 04 — Writer Exclusive Access

**Source:** `../04_writer_exclusive_access.cpp`  
**Binary:** `../bin/04_writer_exclusive_access`

---

## Kya ho raha hai

- Readers: `snap = counter` then sleep 30ms — check `counter == snap`
- Writer: 200ms exclusive, `counter += 10`

Agar read ke beech write ho (bina lock) → `ERROR: value changed during read`.

---

## Advantages

| Fayda | Detail |
|-------|--------|
| Consistent snapshot | Read lock = stable view for critical read |
| No torn reads | Writer blocks all readers |

---

## Run

```bash
./bin/04_writer_exclusive_access
```

`--- exclusive section ---` ke beech readers wait.

---

## Interview

> "Write lock excludes all readers; read locks exclude writer only."

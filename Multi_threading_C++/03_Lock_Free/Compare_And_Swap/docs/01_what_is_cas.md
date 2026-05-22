# 01 — What is CAS

**Source:** `../01_what_is_cas.cpp`  
**Binary:** `../bin/01_what_is_cas`

## Flow

1. `balance = 100`
2. CAS `100 → 80` → **success**
3. CAS `100 → 50` → **fail** (actual 80, `expected` updated to 80)

## Run

```bash
./compile.sh && ./bin/01_what_is_cas
```

## Next

[`02_mutex_vs_atomic_counter.md`](../02_mutex_vs_atomic_counter.md)

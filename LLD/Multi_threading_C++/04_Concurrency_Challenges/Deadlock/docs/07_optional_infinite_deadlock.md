# 07 — Optional Infinite Deadlock

**Source:** `../07_optional_infinite_deadlock.cpp`

## Warning

Uncomment `thread` lines → program **hang forever** — Ctrl+C only.

**Not** in `run_all.sh` by design.

## Manual compile

```bash
g++ -std=c++17 -pthread 07_optional_infinite_deadlock.cpp -o bin/07_hang
# edit main: uncomment threads
./bin/07_hang
```

## Purpose

Feel real deadlock once — then always use fixes 03–05.

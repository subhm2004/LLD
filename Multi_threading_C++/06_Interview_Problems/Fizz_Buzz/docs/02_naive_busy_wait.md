# 02 — Naive Busy Wait

Each thread loops forever until `current` matches its rule, then prints under mutex.

**Downside:** threads **spin** at full CPU when waiting — always say in interview you'd use `condition_variable` or semaphores instead.

```bash
./bin/02_naive_busy_wait
```

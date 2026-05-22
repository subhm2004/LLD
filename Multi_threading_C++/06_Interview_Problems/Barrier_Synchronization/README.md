# Barrier & CountDownLatch

Classic synchronization primitives — often asked with parallel algorithms / phased pipelines.

| # | File | Topic |
|---|------|-------|
| 01 | `01_barrier_demo.cpp` | **Cyclic barrier** — N threads wait for each other |
| 02 | `02_countdown_latch.cpp` | **CountDownLatch** — main waits for N completions |

## Run

```bash
./compile.sh && ./bin/01_barrier_demo
```

## Interview

| Primitive | Use case |
|-----------|----------|
| **Barrier** | Phased parallel compute — all finish step k before step k+1 |
| **CountDownLatch** | Start gun / service ready — one waiter, N signals |
| **Semaphore** | Resource pool permits |

C++20: `std::barrier`, `std::latch` — mention in interview; this repo uses C++17 demos.

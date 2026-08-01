# Bounded Blocking Queue

Implement **enqueue** / **dequeue** with fixed capacity — blocks when full/empty.

| # | File | Topic |
|---|------|-------|
| 01 | `01_producer_consumer_demo.cpp` | `not_full` + `not_empty` condition variables |

## Run

```bash
./compile.sh && ./bin/01_producer_consumer_demo
```

## Modular version

Full pattern module: [`../../02_Concurrency_Patterns/Producer_Consumer_Pattern/`](../../02_Concurrency_Patterns/Producer_Consumer_Pattern/) (`BoundedBuffer.h`)

## Interview

Two CVs on one mutex — same as producer-consumer backpressure.

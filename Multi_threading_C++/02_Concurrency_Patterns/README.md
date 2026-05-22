# Concurrency Patterns (C++17)

Interview-focused concurrency patterns — har pattern ka **alag folder**, runnable `.cpp`, **detailed README**.

| Pattern | Folder | Kya seekhoge |
|---------|--------|--------------|
| **Signaling** | [`Signaling_Pattern/`](./Signaling_Pattern/) | `condition_variable`, notify, wait, shutdown |
| **Thread Pool** | [`Thread_Pool_Pattern/`](./Thread_Pool_Pattern/) | Fixed workers, task queue, reuse, `submit`+`future` |
| **Producer-Consumer** | [`Producer_Consumer_Pattern/`](./Producer_Consumer_Pattern/) | Bounded buffer, SPSC/MPSC/SPMC, backpressure |
| **Reader-Writer** | [`Reader_Writer_Pattern/`](./Reader_Writer_Pattern/) | `shared_mutex`, parallel reads, exclusive writes, cache |

Har folder ka **main README** sabse detailed hai — pattern definition, advantages, har `.cpp` ka flow, interview Q&A.

## Build (kisi bhi pattern folder mein)

```bash
cd Signaling_Pattern && ./compile.sh && ./bin/01_condition_variable_basics
cd Thread_Pool_Pattern && ./compile.sh && ./bin/01_basic_thread_pool
cd Producer_Consumer_Pattern && ./compile.sh && ./bin/01_single_producer_single_consumer
cd Reader_Writer_Pattern && ./compile.sh && ./bin/01_std_shared_mutex_basics
```

## Suggested order

1. **Signaling** — building block (wait/notify)
2. **Producer-Consumer** — signaling on buffer
3. **Thread Pool** — workers + internal queue (producer-consumer inside)
4. **Reader-Writer** — read-heavy shared data (`shared_mutex`)

## Prerequisites & parent

- [`../01_Fundamentals/`](../01_Fundamentals/) — mutex, `condition_variable`
- [`../README.md`](../README.md) — full module index

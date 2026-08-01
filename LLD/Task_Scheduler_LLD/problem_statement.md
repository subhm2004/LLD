# Problem Statement: Task / Job Scheduler

Design a low-level **task (job) scheduler** that accepts background jobs, runs them on a **worker pool**, supports **delayed execution**, **priority**, **retries**, and **cancellation**.

## Functional requirements

1. **Submit** a job with name, executable task, priority, optional delay, and retry policy.
2. **Schedule** jobs to run at `now + delayMs` (in-memory clock).
3. **Execute** jobs on a pool of **N worker threads**.
4. Pick ready jobs using **priority** (CRITICAL > HIGH > NORMAL > LOW).
5. On failure, **retry** up to `maxRetries` with a configurable delay between attempts.
6. **Cancel** a pending job before it starts running.
7. Notify **observers** on status transitions (Observer pattern).
8. Query job **status** and metadata after submission.

## Non-functional

- In-memory only (interview scope).
- C++17, header-based modules, `compile.sh` + pthread.
- Deterministic demo via `SimulatedClock` (advance time from `main`).

## Out of scope

- Distributed scheduling (Redis, Quartz cluster)
- Persistent job store / crash recovery
- Real cron expression parser (use delay ms or extend with CronStrategy)
- DAG / workflow dependencies between jobs

## Assumptions

- One process, shared memory.
- Job task is a `std::function<void()>`; failures throw `std::exception`.
- Idempotent retry is the caller's responsibility.

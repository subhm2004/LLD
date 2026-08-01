# Requirements: Task / Job Scheduler

## Functional (FR)

| ID | Requirement |
|----|-------------|
| FR-1 | Submit job with priority, delay, max retries |
| FR-2 | Worker pool with configurable thread count |
| FR-3 | Delayed jobs run only when `scheduledAt <= clock.now()` |
| FR-4 | Priority queue for ready jobs |
| FR-5 | Retry with backoff delay on exception |
| FR-6 | Cancel non-terminal jobs |
| FR-7 | Observer callbacks on status change |

## Non-functional (NFR)

| ID | Requirement |
|----|-------------|
| NFR-1 | Thread-safe delayed + ready queues |
| NFR-2 | C++17, `-pthread` |
| NFR-3 | Simulated clock for reproducible demos |

## Design patterns

| Pattern | Where |
|---------|--------|
| **Facade** | `TaskSchedulerSystem` |
| **Strategy** | `ISchedulingStrategy`, `PrioritySchedulingStrategy`, `FifoSchedulingStrategy` |
| **Observer** | `IJobObserver`, `ConsoleJobObserver` |
| **Factory** | `JobFactory` |
| **Service layer** | Registry, Scheduler, Retry, Worker pool |

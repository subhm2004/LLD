# 05 — Classic Problems (Legacy Monoliths)

Single-file demos jo modular folders se **pehle** likhe gaye. Naye learners ko pehle numbered modules follow karne chahiye; yeh files **reference / compare** ke liye hain.

| Problem | Folder | Modular alternative |
|---------|--------|---------------------|
| Dining Philosophers | [`Dining_Philosophers/`](./Dining_Philosophers/) | — (unique) |
| Producer-Consumer | [`Producer_Consumer_Legacy/`](./Producer_Consumer_Legacy/) | [`../02_Concurrency_Patterns/Producer_Consumer_Pattern/`](../02_Concurrency_Patterns/Producer_Consumer_Pattern/) |
| Thread Pool | [`Thread_Pool_Legacy/`](./Thread_Pool_Legacy/) | [`../02_Concurrency_Patterns/Thread_Pool_Pattern/`](../02_Concurrency_Patterns/Thread_Pool_Pattern/) |
| Deadlock | [`Deadlock_Legacy/`](./Deadlock_Legacy/) | [`../04_Concurrency_Challenges/Deadlock/`](../04_Concurrency_Challenges/Deadlock/) |
| DCLP | [`Double_Checked_Locking/`](./Double_Checked_Locking/) | — |

## Compile (example)

```bash
g++ -std=c++17 -pthread Dining_Philosophers/dining_philosophers.cpp -o dp && ./dp
```

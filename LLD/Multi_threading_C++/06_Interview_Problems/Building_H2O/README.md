# Building H2O — LeetCode 1117

Hydrogen and oxygen threads arrive randomly; form **H₂O** molecules (2 H + 1 O per bond).

| # | File | Approach |
|---|------|----------|
| 01 | `01_bond_molecules.cpp` | Counters + `condition_variable` wait for `h==2 && o>=1` |

## Run

```bash
./compile.sh && ./bin/01_bond_molecules
```

## Interview one-liner

Wait until resources available → second wait until ratio 2:1 → release atom → last H resets counters.

## Related

Resource counting (semaphore-like) — compare [`../../02_Concurrency_Patterns/Producer_Consumer_Pattern/`](../../02_Concurrency_Patterns/Producer_Consumer_Pattern/)

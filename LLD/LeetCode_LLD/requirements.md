# Requirements: LeetCode_LLD

## Functional

1. `registerUser(userId, displayName)`
2. `addProblem(problem)` with test cases
3. `searchProblemsByTag(tag)` / `searchProblemsByDifficulty(difficulty)`
4. `submitSolution(userId, problemId, language, code)` → `Submission` with final status
5. `getLeaderboard(limit)` sorted by solved count
6. `getUserSubmissions(userId)` history

## Non-functional

1. C++17, modular headers under `core/`, `models/`, `services/`, `enums/`
2. In-memory storage (`unordered_map`)
3. Judge pipeline extensible via `ICodeRunner` (DIP)
4. Demo uses `MockCodeRunner` — no real code execution

## Design patterns

| Pattern | Where |
| ------- | ----- |
| **Facade** | `LeetCodeSystem` |
| **Strategy** | `ICodeRunner` / `MockCodeRunner` |
| **Service layer** | Catalog, Submission, Judge, Leaderboard |

## Verdict rules (demo)

- All test cases must match expected output → `ACCEPTED`
- Mismatch → `WRONG_ANSWER` + failed test case id
- Code contains `COMPILE_ERROR` → `COMPILE_ERROR`
- Code contains `TLE_MARKER` → `TIME_LIMIT_EXCEEDED`
- Other execution throws → `RUNTIME_ERROR`

Correct solutions must include marker comments: `SOLUTION_TWO_SUM`, `SOLUTION_REVERSE`, `SOLUTION_VALID_PAREN`, `SOLUTION_MIN_COST_DIVIDE`.

## MIN_COST_DIVIDE_ARRAY test I/O format

```
nums=3,1,4;cost=4,6,6;k=1   →  110
```

Reference implementation: `algorithms/MinCostDivideArraySolver.h` (optimized DP O(n²)).  
Full LeetCode-style variants: `algorithms/MinCostDivideArray_ReferenceSolution.h`.

# LeetCode_LLD

LeetCode-style **online judge** — problem catalog, multi-language submissions, automated judging, leaderboard.

## Structure

```
LeetCode_LLD/
├── core/LeetCodeSystem.h      # Facade
├── models/                    # User, Problem, Submission, TestCase
├── services/                  # Catalog, Submission, Judge, Leaderboard
├── enums/                     # Difficulty, SubmissionStatus, Language
├── main.cpp
├── compile.sh
└── problem_statement.md
```

## Patterns

- **Facade** — `LeetCodeSystem`
- **Strategy** — `ICodeRunner` (swap real sandbox later)
- **Service layer** — catalog, judge, submissions

## Build & run

```bash
cd LeetCode_LLD
./compile.sh
./leetcode_app
```

## Demo problems (seeded)

| ID | Title | Difficulty |
|----|-------|------------|
| `TWO_SUM` | Two Sum | EASY |
| `REVERSE_STRING` | Reverse String | EASY |
| `VALID_PARENTHESES` | Valid Parentheses | MEDIUM |
| `MIN_COST_DIVIDE_ARRAY` | Minimum Cost to Divide Array Into Subarrays | HARD |

### HARD — `MIN_COST_DIVIDE_ARRAY`

Full statement: [`problems/MIN_COST_DIVIDE_ARRAY.md`](./problems/MIN_COST_DIVIDE_ARRAY.md)

**Formula (i-th subarray, 1-based):**

`(sum(nums[0..r]) + k * i) * (sum(cost[l..r]))`

**DP:** `O(n²)` optimized — `algorithms/MinCostDivideArraySolver.h`  
**Reference (your LeetCode solution):** `algorithms/MinCostDivideArray_ReferenceSolution.h`

**Submit marker:** `SOLUTION_MIN_COST_DIVIDE` in code for AC on hidden tests.

## Interview extensions

- Contest module with ranking
- Editorial / discussion service
- `Repository` for SQL persistence
- Real judge queue (Kafka + worker pool)

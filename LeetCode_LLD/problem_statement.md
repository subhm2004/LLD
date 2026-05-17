# Problem Statement: LeetCode-Like Online Judge (LLD)

Design an in-memory coding platform where users can browse problems, submit solutions in multiple languages, and receive judged results.

## Functional requirements

1. Register users.
2. Maintain a catalog of coding problems (title, difficulty, tags, test cases).
3. Search problems by tag or difficulty.
4. Submit code for a problem; run against hidden + sample test cases.
5. Return verdict: `ACCEPTED`, `WRONG_ANSWER`, `TLE`, `RUNTIME_ERROR`, `COMPILE_ERROR`.
6. Track per-user solved count and leaderboard.
7. Track per-problem acceptance rate.

## Out of scope (HLD / production)

- Real compiler / sandbox
- Distributed judge workers
- Persistent database
- Contests, discussions, premium subscriptions

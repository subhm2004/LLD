# CricBuzz Live Cricket Scoring LLD (C++17)

In-memory **live cricket scoring** — teams, schedule match, toss, ball-by-ball updates, live scoreboard, commentary feed.

## Quick start

```bash
./compile.sh
./cricbuzz_app
```

## Docs

| File | Purpose |
|------|---------|
| [`problem_statement.md`](./problem_statement.md) | Scope |
| [`requirements.md`](./requirements.md) | FR / NFR |

## Architecture

```
CricBuzz_LLD/
├── core/CricBuzzSystem.h           # Facade
├── services/
│   ├── TeamRegistryService.h
│   ├── MatchService.h
│   ├── ScoringService.h
│   └── LiveScoreService.h
├── commentary/
│   ├── ICommentaryGenerator.h      # Strategy
│   └── DefaultCommentaryGenerator.h
└── main.cpp
```

## Main APIs

| API | Description |
|-----|-------------|
| `registerTeam` / `registerPlayer` | Registry |
| `scheduleMatch` | T20 / ODI / TEST |
| `conductToss` / `startMatch` | Begin 1st innings |
| `recordBall` | Ball-by-ball scoring |
| `getLiveScore` | Scoreboard + RR + target |
| `getCommentaryFeed` | Last N ball comments |
| `startSecondInnings` / `endMatch` | Chase + result |

## Design patterns

| Pattern | Where |
|---------|--------|
| **Facade** | `CricBuzzSystem` |
| **Strategy** | `ICommentaryGenerator` |
| **Service layer** | Match / Scoring / LiveScore |

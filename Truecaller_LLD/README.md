# Truecaller LLD (C++17)

Caller identification, contact enrichment, spam reporting, and block list — interview-style low-level design.

## Quick start

```bash
./compile.sh
./truecaller_app
```

## Docs

| File | Purpose |
|------|---------|
| [`problem_statement.md`](./problem_statement.md) | Problem scope + assumptions |
| [`requirements.md`](./requirements.md) | Functional / non-functional requirements |

## Architecture

```
Truecaller_LLD/
├── core/TruecallerSystem.h      # Facade — single entry point
├── models/                      # User, PhoneProfile, ContactEntry, CallLog, LookupResult
├── enums/                       # CallerTag, ReportReason, CallType, AccountStatus
├── services/                    # Lookup, ContactSync, Spam, Block, Search, CallLog
├── strategies/                  # ISpamScoringStrategy
└── utils/PhoneUtils.h           # normalize + case helpers
```

## Main APIs (`TruecallerSystem`)

| API | Description |
|-----|-------------|
| `registerUser` | Register + publish phone to global directory |
| `updateProfile` | Update name / business flag |
| `identifyCaller` | Caller ID for incoming number |
| `syncContacts` | Enrich uploaded contacts from directory |
| `reportSpam` | Increase spam score (Strategy) |
| `blockNumber` / `unblockNumber` | Per-user block list |
| `searchByName` / `searchByPhone` | Directory search |
| `logCall` / `getCallHistory` | Call log |

## Design patterns

| Pattern | Where |
|---------|--------|
| **Facade** | `TruecallerSystem` |
| **Strategy** | `DefaultSpamScoringStrategy` — score delta per report reason |
| **Service layer** | Lookup, Block, Search, etc. |

## Interview extensions

- Shard directory by phone prefix; cache hot lookups in Redis.
- Crowd-sourced spam ML model behind `ISpamScoringStrategy`.
- Privacy: opt-out from directory, show only “Spam” without name.
- Thread-safe reads with `shared_mutex` on directory map.

## Related LLD

- [OTP Generation](../OTP_Generation_System_LLD/) — phone + notification
- [LinkedIn](../Linkedin_LLD/) — social graph + search
- [WhatsApp](../WhatsApp_LLD/) — contacts + messaging

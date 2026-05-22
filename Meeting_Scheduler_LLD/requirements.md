# Meeting Scheduler — Requirements

## Functional

| ID | Requirement |
|----|-------------|
| FR1 | Register user with unique email |
| FR2 | Add availability window on a date (start < end, within day) |
| FR3 | Schedule meeting with organizer + ≥1 attendee |
| FR4 | Reject if any participant has overlapping meeting |
| FR5 | Reject if slot outside any participant availability |
| FR6 | Cancel meeting (participant must be on invite list or organizer) |
| FR7 | List meetings for user filtered by date |
| FR8 | Find mutual free slots of given duration on a date |

## Non-functional

| ID | Requirement |
|----|-------------|
| NFR1 | C++17, header-only services, `compile.sh` runnable demo |
| NFR2 | Clear error messages (`std::runtime_error` / `invalid_argument`) |
| NFR3 | Extensible slot-finding via **Strategy** |

## Patterns

| Pattern | Use |
|---------|-----|
| **Facade** | `MeetingSchedulerSystem` |
| **Strategy** | `IFreeSlotStrategy` — earliest mutual slot scan |
| **Factory** | `MeetingFactory` |
| **Service layer** | Availability, conflict, booking, slot finder |

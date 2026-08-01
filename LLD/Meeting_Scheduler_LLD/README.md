# Meeting Scheduler LLD (C++17)

Calendly-style **meeting scheduler**: availability windows, conflict-safe booking, mutual free-slot discovery.

## Quick start

```bash
./compile.sh
./meeting_scheduler_app
```

## Docs

| File | Purpose |
|------|---------|
| [`problem_statement.md`](./problem_statement.md) | Scope + assumptions |
| [`requirements.md`](./requirements.md) | FR / NFR + patterns |

## Architecture

```
Meeting_Scheduler_LLD/
├── core/MeetingSchedulerSystem.h   # Facade
├── models/                         # User, Meeting, AvailabilityWindow, TimeSlot
├── enums/MeetingStatus.h
├── services/                       # Availability, Conflict, Booking, SlotFinder
├── strategies/                     # EarliestMutualSlotStrategy
├── factories/MeetingFactory.h
└── utils/TimeUtils.h
```

## Main APIs

| API | Description |
|-----|-------------|
| `registerUser` | Name, email (unique), timezone label |
| `setAvailability` | Per-day window (minutes from midnight) |
| `scheduleMeeting` | Organizer + attendees; conflict + availability checks |
| `cancelMeeting` | Any participant |
| `listMeetingsForUser` | Optional date filter |
| `findMutualFreeSlots` | Strategy scans 15-min steps |

## Design patterns

| Pattern | Use |
|---------|-----|
| **Facade** | `MeetingSchedulerSystem` |
| **Strategy** | `IFreeSlotStrategy` / `EarliestMutualSlotStrategy` |
| **Factory** | `MeetingFactory` |
| **Service layer** | Availability, conflict detection, booking |

## Interview extensions

- Recurring availability (weekly RRULE)
- Timezone conversion per user
- Room/resource booking as extra participant
- Reminder queue + ICS email

**UML:** [§27 Meeting Scheduler](../docs/SYSTEM_CLASS_AND_SEQUENCE_DIAGRAMS.md#27-meeting-scheduler)

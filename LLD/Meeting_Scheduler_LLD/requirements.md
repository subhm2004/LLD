# Meeting Scheduler — Requirements

## Core Requirements (R1–R6) — Problem Statement se

Ye woh 6 core requirements hain jo problem definition me diye gaye the. Neeche har requirement
ke saamne uska code location aur pattern mapping diya gaya hai — matlab har point actually
implement ho chuka hai, sirf theory nahi.

| ID | Requirement | Kahan implement hua (code) |
|----|-------------|-----------------------------|
| **R1** | System me ek fixed number ke meeting rooms available hone chahiye | `MeetingRoom` model + `MeetingSchedulerSystem::addMeetingRoom()` + `rooms_` map |
| **R2** | Har room ki ek capacity hoti hai (kitne log baith sakte hain) | `MeetingRoom::capacity_` + `MeetingRoom::canAccommodate()` + `ConflictDetectionService::validateRoom()` |
| **R3** | Room tabhi reserve ho jab wo already booked na ho — fixed start/end time ke saath | `ConflictDetectionService::validateRoom()` (overlap check) + `Meeting::overlaps()` |
| **R4** | Meeting me invited sabhi logon ko notification bhejna zaruri hai | **Observer** — `NotificationService` (Subject) + `INotificationObserver` / `ConsoleNotificationObserver` |
| **R5** | Sab invitees ko invitation mile (available ho ya na ho), aur wo Accept/Decline kar sakein | `Meeting::attendeeStatuses_` (PENDING/ACCEPTED/DECLINED) + `respondToInvitation()` + `strictMode=false` flexible flow |
| **R6** | Har user ke paas ek personal Calendar ho jo dates/times track kare + schedule/cancel manage kare | `User::calendar_` (`Calendar` model) — `addMeeting` / `removeMeeting` / `addAvailability` |

### Problem Definition ke extra points

- **Organizer schedule ke baad bhi naye log add kar sakta hai** → `Meeting::attendeeStatuses_` map extensible hai; naya attendee PENDING status ke saath add ho sakta hai.
- **Participants ko quick notifications milein** → Observer pattern (`NotificationService`) broadcast turant kar deta hai.
- **Design patterns discuss karo** → dekho [`design_patterns_used.md`](./design_patterns_used.md).

---

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

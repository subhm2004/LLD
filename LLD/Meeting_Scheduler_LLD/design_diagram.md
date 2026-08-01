# Meeting Scheduler LLD — Design Diagrams

> Codebase padh ke banaya. Yeh ek rich system hai: **Strategy** (free-slot finding),
> **Observer** (notifications), **Factory** (meeting), aur 5 services — sab interval-overlap
> math ke upar khade hain.

---

## 1. Class Diagram

```mermaid
classDiagram
    direction TB

    class MeetingSchedulerSystem {
        <<Facade>>
        -Map users_
        -Map emailToUserId_
        -Map rooms_
        -Map availability_
        -Map meetings_
        -AvailabilityService availabilityService_
        -ConflictDetectionService conflictService_
        -BookingService bookingService_
        -SlotFinderService slotFinderService_
        -NotificationService notificationService_
        +registerUser(name, email, tz) userId
        +setAvailability(userId, date, start, end)
        +scheduleMeeting(org, attendees, ...) Meeting
        +cancelMeeting(id, requester)
        +findMutualFreeSlots(userIds, date, duration) TimeSlot[]
        +respondToInvitation(id, userId, status)
    }

    class BookingService {
        -ConflictDetectionService& conflictService_
        +schedule(...) Meeting
        +cancel(...)
    }
    class ConflictDetectionService {
        +validateRoom(...)
        +validateSchedule(...)
        +hasMeetingConflict(...) bool
        +isWithinAvailability(...) bool
    }
    class AvailabilityService {
        +addWindow(...)
        +listForUser(...) AvailabilityWindow[]
    }
    class SlotFinderService {
        -shared_ptr~IFreeSlotStrategy~ strategy_
        +findMutualSlots(...) TimeSlot[]
    }
    class NotificationService {
        -INotificationObserver[] observers_
        +registerObserver(obs)
        +notifyScheduled(Meeting)
        +notifyCancelled(Meeting, requester)
    }

    class IFreeSlotStrategy {
        <<interface>>
        +findSlots(...)* TimeSlot[]
    }
    class EarliestMutualSlotStrategy

    class INotificationObserver {
        <<interface>>
        +onMeetingScheduled(Meeting)*
        +onMeetingCancelled(...)*
        +onInvitationUpdated(...)*
    }
    class ConsoleNotificationObserver

    class MeetingFactory {
        <<static>>
        +create(...)$ Meeting
    }

    class Meeting {
        -string meetingId_
        -string organizerId_
        -string[] attendeeIds_
        -string date_
        -int startMinutes_
        -int endMinutes_
        -string roomId_
        -MeetingStatus status_
        -Map attendeeStatuses_
        +involvesUser(id) bool
        +overlaps(start, end) bool
        +cancel()
    }
    class User {
        -string userId_
        -string email_
        -Calendar calendar_
    }
    class Calendar {
        -AvailabilityWindow[] availabilityWindows_
        -Set meetingIds_
    }
    class MeetingRoom {
        -int capacity_
        +canAccommodate(n) bool
    }
    class AvailabilityWindow {
        -int startMinutes_
        -int endMinutes_
        +contains(s, e) bool
        +overlaps(s, e) bool
    }
    class TimeSlot {
        -string date_
        -int startMinutes_
        -int endMinutes_
    }

    IFreeSlotStrategy <|.. EarliestMutualSlotStrategy
    INotificationObserver <|.. ConsoleNotificationObserver
    SlotFinderService o-- IFreeSlotStrategy
    NotificationService o-- INotificationObserver
    BookingService o-- ConflictDetectionService
    BookingService ..> MeetingFactory : creates via

    MeetingSchedulerSystem *-- User
    MeetingSchedulerSystem *-- MeetingRoom
    MeetingSchedulerSystem *-- Meeting
    MeetingSchedulerSystem *-- BookingService
    MeetingSchedulerSystem *-- AvailabilityService
    MeetingSchedulerSystem *-- SlotFinderService
    MeetingSchedulerSystem *-- NotificationService
    User *-- Calendar
    MeetingFactory ..> Meeting : builds
```

---

## 2. ⭐ Foundation — interval overlap math (TimeUtils)

Poora system in do functions pe khada hai (time = minutes-since-midnight, `[start, end)`):

```cpp
rangesOverlap(aStart, aEnd, bStart, bEnd) = aStart < bEnd && bStart < aEnd
rangeContains(outStart, outEnd, inStart, inEnd) = outStart <= inStart && inEnd <= outEnd
```

```mermaid
flowchart LR
    subgraph rangesOverlap
    O["aStart before bEnd AND bStart before aEnd"]
    end
    subgraph rangeContains
    C["outer fully covers inner slot"]
    end
```

| Use | Function | Kahan |
|---|---|---|
| Do meeting takraati hain? | `rangesOverlap` | conflict detection, room booking |
| Slot availability window ke ANDAR hai? | `rangeContains` | `AvailabilityWindow::contains` |

> **Half-open `[start, end)`:** 10:00–11:00 aur 11:00–12:00 **takraati NAHI** (`11:00 < 11:00` = false).
> Yahi wajah hai `<` use hota hai `<=` nahi — back-to-back meetings valid hain.

---

## 3. ⭐ Free-slot finding (Strategy) — EarliestMutualSlot

```mermaid
flowchart TD
    A[findSlots date, duration, userIds] --> B[sab users ki availability ka<br/>UNION nikalo: dayStart..dayEnd]
    B --> C{koi availability hai?}
    C -- nahi --> Z[empty list]
    C -- haan --> D[start = dayStart se<br/>15-min step pe aage badho]
    D --> E{har user ke liye:<br/>slot availability ke andar?<br/>AND koi meeting conflict nahi?}
    E -- sab free --> F[slot list me daalo]
    E -- koi busy --> G[skip]
    F --> H{start + duration <= dayEnd?}
    G --> H
    H -- haan --> D
    H -- nahi --> R[mutual free slots return]
```

> **Strategy kyun?** Abhi "earliest mutual slot" (15-min grid) hai. Kal ko "least fragmentation",
> "prefer mornings", ya "AI-optimized" chahiye to sirf nayi `IFreeSlotStrategy` class —
> `SlotFinderService` aur baaki system untouched.

---

## 4. Sequence — scheduleMeeting (poora flow)

```mermaid
sequenceDiagram
    autonumber
    actor Organizer
    participant Sys as MeetingSchedulerSystem
    participant BS as BookingService
    participant CD as ConflictDetectionService
    participant Fac as MeetingFactory
    participant NS as NotificationService

    Organizer->>Sys: scheduleMeeting(org, attendees, date, time, roomId?, strict)
    Sys->>Sys: validateUser(org + attendees)
    Sys->>BS: schedule(...)
    alt roomId khaali
        BS->>BS: auto-assign: capacity + no-conflict wala room dhoondo
    else roomId diya
        BS->>CD: validateRoom(capacity + double-booking check)
    end
    BS->>CD: validateSchedule(organizer availability + conflicts)
    Note over CD: strictMode -> attendees ka bhi<br/>availability + conflict check
    CD-->>BS: ok (ya throw)
    BS->>Fac: create(meetingId, ...)
    Fac-->>BS: Meeting (attendees = PENDING)
    BS->>BS: organizer + attendees ke Calendar me meetingId add
    BS->>NS: notifyScheduled(meeting)
    NS-->>Organizer: [NOTIFICATION] Meeting scheduled
    BS-->>Sys: Meeting
```

---

## 5. State diagrams — Meeting & Invitation

```mermaid
stateDiagram-v2
    direction LR
    state "Meeting" as M {
        [*] --> SCHEDULED
        SCHEDULED --> CANCELLED : cancel()
        CANCELLED --> [*]
    }
    state "Invitation (per attendee)" as I {
        [*] --> PENDING
        PENDING --> ACCEPTED : respond (conflict-free)
        PENDING --> DECLINED : respond
        ACCEPTED --> DECLINED : respond
    }
```

> ⭐ **Accept pe conflict check:** invitation ACCEPT karte waqt bhi `hasMeetingConflict`
> chalta hai (current meeting ko skip karke) — taaki koi do overlapping meetings accept na kar le.
> DECLINE hamesha allowed hai. Aur **DECLINED attendee ko conflict-check me ignore** kiya jaata
> hai (usne mana kiya to us slot pe wo free maana jaata hai).

---

## 6. Design patterns summary

| Pattern | Kahan | Kyun |
|---|---|---|
| **Facade** | `MeetingSchedulerSystem` | ek darwaza, 5 services andar |
| **Strategy** | `IFreeSlotStrategy` → `EarliestMutualSlotStrategy` | slot-finding algorithm swappable |
| **Observer** | `INotificationObserver` → `ConsoleNotificationObserver` | schedule/cancel/response pe notify; email/SMS observer add ho sakte |
| **Factory** | `MeetingFactory` | Meeting banane ka ek jagah |
| **Service layer / SRP** | Availability, Conflict, SlotFinder, Booking, Notification | ek class ek zimmedari |
| **Dependency Injection** | `BookingService(conflictService)`, strategy inject | testable, loosely coupled |

> ⭐ **Data ownership:** saara state (`users_`, `rooms_`, `availability_`, `meetings_`) facade ke
> paas hai; services **stateless** hain aur data **parameters se** lete hain — isliye unhe test
> karna aasan aur inme koi hidden state nahi.

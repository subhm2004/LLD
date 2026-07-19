# Design Patterns Used — Meeting Scheduler LLD

Meeting Scheduler ka code **clean, decoupled, extensible aur testable** rahe — iske liye kayi
GoF design patterns aur ek layered service architecture ka use kiya gaya hai. Neeche har pattern
ko uske **role, code location, kaam, aur requirement mapping (R1–R6)** ke saath detail me samjhaya
gaya hai.

```
┌──────────────────────────────────────────────────────────────────────┐
│                         PATTERN OVERVIEW                               │
├──────────────┬──────────────────────────────┬────────────────────────┤
│  Pattern     │  Kahan (class)               │  Kis requirement ke liye│
├──────────────┼──────────────────────────────┼────────────────────────┤
│  Facade      │  MeetingSchedulerSystem      │  Poore system ka entry  │
│  Strategy    │  IFreeSlotStrategy           │  R6 / FR8 slot search   │
│  Factory     │  MeetingFactory              │  Meeting object banana  │
│  Observer    │  NotificationService         │  R4 notifications       │
│  Service     │  Availability/Conflict/...    │  R1-R5 business rules   │
│  Layer       │  Booking/SlotFinder          │  (architectural)        │
└──────────────┴──────────────────────────────┴────────────────────────┘
```

---

## 1. Facade Design Pattern

### 📌 Concept
Facade ek complex subsystem (bahut saari internal classes/services) ke aage ek **single,
simplified interface** rakh deta hai. Client ko andar ke moving parts se matlab nahi rehta.

### PATTERN KE ROLES
- **Facade** → [`MeetingSchedulerSystem`](./core/MeetingSchedulerSystem.h)
- **Subsystem classes** → `AvailabilityService`, `ConflictDetectionService`, `BookingService`, `SlotFinderService`, `NotificationService`

### Implementation
- Client (jaise [`main.cpp`](./main.cpp)) ko individually 5 services ko banane, link karne ya sahi order me call karne ki tension nahi leni padti.
- Client sirf itna karta hai:
  ```cpp
  MeetingSchedulerSystem scheduler;
  scheduler.addMeetingRoom("ROOM_A", "Grand Ballroom", 10);
  auto uid = scheduler.registerUser("Alice", "alice@work.com");
  scheduler.scheduleMeeting(uid, {bobId}, "Sprint planning", date, 600, 660, "ROOM_B");
  ```
- Facade constructor internally dependency wiring karta hai — `bookingService_(conflictService_)` — taaki services aapas me connected rahein but client ko dikhe nahi.

### ⭐ KYUN? (Why Facade)
> 5 services ka orchestration ek jagah aa gaya. Kal ko internal service signatures badlein bhi,
> to client code (main.cpp) untouched rahega — sirf facade adjust hoga. **Loose coupling.**

---

## 2. Strategy Design Pattern

### 📌 Concept
Strategy ek algorithm ko uske use karne wale se alag ("interchangeable") bana deta hai. Alag-alag
algorithm same interface follow karte hain, aur runtime pe swap ho sakte hain.

### PATTERN KE ROLES
- **Strategy interface** → [`IFreeSlotStrategy`](./strategies/IFreeSlotStrategy.h)
- **Concrete Strategy** → [`EarliestMutualSlotStrategy`](./strategies/EarliestMutualSlotStrategy.h) (15-min steps pe linear scan)
- **Context** → [`SlotFinderService`](./services/SlotFinderService.h) — jo `shared_ptr<IFreeSlotStrategy>` hold karta hai

### Implementation
- `SlotFinderService` ke do constructor hain: default (`EarliestMutualSlotStrategy` inject karta hai) aur ek jo bahar se koi bhi strategy le leta hai.
- Free-slot dhoondhne ka poora logic strategy ke andar band hai — context sirf `strategy_->findSlots(...)` call karta hai.

### ⭐ KYUN? (Why Strategy)
> Aaj simple linear scan (O(day × users)) chal raha hai. Kal ko **interval tree** ya **binary
> search** based `IntervalTreeSlotStrategy` chahiye — to bas naya class banao aur inject kar do.
> `SlotFinderService` ya baaki system ka ek line bhi change nahi hoga. **Open/Closed Principle.**

---

## 3. Factory Design Pattern (Simple Factory)

### 📌 Concept
Object construction ki logic ko ek dedicated jagah (factory) me centralize karo, taaki client ko
`new`/constructor ki details na jhelni padein.

### PATTERN KE ROLES
- **Factory** → [`MeetingFactory`](./factories/MeetingFactory.h) (static `create(...)`)
- **Product** → [`Meeting`](./models/Meeting.h)

### Implementation
- [`BookingService::schedule()`](./services/BookingService.h) directly `new Meeting(...)` nahi karti — wo `MeetingFactory::create(meetingId, organizerId, attendeeIds, ...)` call karti hai.

### ⭐ KYUN? (Why Factory)
> Kal ko `Meeting` construction me extra step aa jaye (audit-log, default reminder, ID scheme
> change) — to sirf factory ke ek method me change karo. Har jagah jahan meeting banti hai, wahan
> jaakar edit karne ki zarurat nahi. **Single point of change.**

---

## 4. Observer Design Pattern  ⭐ (R4 ka dil)

### 📌 Concept
One-to-Many dependency: jab ek **Subject** ka state change hota hai, uske saare registered
**Observers** ko automatically notify kar diya jata hai — Subject ko ye tak nahi pata ki observer
kaun hai aur kya karega.

### PATTERN KE ROLES
- **Subject** → [`NotificationService`](./services/NotificationService.h) — observers ki list rakhta hai + `notifyScheduled` / `notifyCancelled` / `notifyInvitationUpdated`
- **Observer interface** → `INotificationObserver` (`onMeetingScheduled`, `onMeetingCancelled`, `onInvitationUpdated`)
- **Concrete Observer** → `ConsoleNotificationObserver` (console pe print karta hai)

### Implementation flow
```
scheduleMeeting()                 respondToInvitation()
      │                                   │
      ▼                                   ▼
BookingService.schedule()      Meeting.setAttendeeStatus()
      │                                   │
      ▼                                   ▼
NotificationService.notifyScheduled()  .notifyInvitationUpdated()
      │  (loop over observers)           │
      ▼                                   ▼
ConsoleNotificationObserver  ──►  [NOTIFICATION] ... print
```

### ⚠️ R5 se link
Jab koi attendee `respondToInvitation(..., ACCEPTED/DECLINED)` karta hai, wahi event bhi observer
ko broadcast hota hai — matlab organizer ko turant pata chal jata hai kisne accept/decline kiya.

### ⭐ KYUN? (Why Observer)
> Kal ko console ke alawa **Email**, **Slack**, ya **Push** notification chahiye? Bas ek naya
> `EmailNotificationObserver : INotificationObserver` bana ke `registerObserver()` kar do.
> `NotificationService` aur baaki system ko chhuna nahi padega. **Notification channels pluggable.**

---

## 5. Service Layer (Architectural Pattern)

### 📌 Concept
Business logic ko chhoti, **single-responsibility** services me tod do. Facade orchestrate karta
hai; har service apna ek hi kaam achhe se karti hai.

| Service | Zimmedari (Single Responsibility) | Requirement |
|---------|-----------------------------------|-------------|
| [`AvailabilityService`](./services/AvailabilityService.h) | User ki availability windows add/list + overlap validate | FR2 |
| [`ConflictDetectionService`](./services/ConflictDetectionService.h) | Room capacity + room double-booking + user schedule clash checks | R1, R2, R3, FR4, FR5 |
| [`BookingService`](./services/BookingService.h) | Room assign, factory se meeting banao, calendar update, notify | R1–R4 orchestration |
| [`SlotFinderService`](./services/SlotFinderService.h) | Strategy ke through mutual free slots dhoondho | FR8, R6 |
| [`NotificationService`](./services/NotificationService.h) | Observers ko events broadcast karo | R4, R5 |

### ⭐ KYUN? (Why Service Layer)
> Har rule alag class me hai → **test karna, badalna aur padhna aasaan.** Ek jagah ka bug doosri
> jagah nahi failata. Facade patla rehta hai, services reusable rehti hain.

---

## Object Relationships (UML samajhne ke liye)

```
MeetingSchedulerSystem  ◆──  AvailabilityService        (Composition: facade owns services)
MeetingSchedulerSystem  ◆──  ConflictDetectionService
MeetingSchedulerSystem  ◆──  BookingService
BookingService          ──▶  ConflictDetectionService   (Association: uses reference)
BookingService          ··▶  MeetingFactory             (Dependency: static call)
SlotFinderService       ◇──  IFreeSlotStrategy          (Aggregation: injected, shared_ptr)
NotificationService     ◇──  INotificationObserver      (Aggregation: registered observers)
User                    ◆──  Calendar                   (Composition: user owns calendar)
```

- `◆──` **Composition** (strong "owns-a": lifetime bandha hua)
- `◇──` **Aggregation** (weak "has-a": bahar se inject, independent lifetime)
- `──▶` **Association** ("uses-a" via stored reference)
- `··▶` **Dependency** ("uses-a" temporarily / static call)

---

## Requirement → Pattern quick map

| Requirement | Pattern / Mechanism |
|-------------|---------------------|
| R1 rooms available | Facade + `rooms_` store |
| R2 room capacity | `MeetingRoom.canAccommodate` + ConflictDetectionService |
| R3 reserve if free | ConflictDetectionService overlap check |
| R4 notify invitees | **Observer** (NotificationService) |
| R5 invite + accept/decline | `attendeeStatuses_` + Observer broadcast |
| R6 per-user calendar | `User ◆── Calendar` composition |
| FR8 mutual slots | **Strategy** (IFreeSlotStrategy) |
| Object creation | **Factory** (MeetingFactory) |
| Simplified entry | **Facade** (MeetingSchedulerSystem) |

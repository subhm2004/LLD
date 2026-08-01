# Design Patterns Used — Airline Management System LLD

Is system me flight operations, bookings, payments, crew, aur role-based access —
sab kuch **clean, decoupled aur thread-safe** rahe, iske liye do GoF patterns
(Facade, Strategy), ek layered service architecture (SRP), aur do concurrency/access
techniques (mutex critical section, ordinal RBAC) use hui hain.

> 📌 Note: ye file `.cpp` naam se maangi gayi thi, par ye **documentation** hai (compile
> hone wali C++ nahi) — isi liye ise `.md` rakha gaya hai, taaki repo ke baaki folders
> (Meeting_Scheduler, vending_machine, Linkedin) ke `design_patterns_used.md` ke saath
> consistent rahe.

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                            PATTERN OVERVIEW                                    │
├──────────────────┬─────────────────────────────────┬─────────────────────────┤
│  Pattern         │  Kahan (class)                  │  Kis liye                │
├──────────────────┼─────────────────────────────────┼─────────────────────────┤
│  Facade          │  AirlineManagementSystem        │  Poore system ka entry   │
│  Strategy        │  IPricingStrategy               │  Seat fare calculation   │
│  Service Layer   │  FlightSearch/SeatInventory/    │  SRP, decoupled logic    │
│  (SRP)           │  Crew/Payment/Notification      │                          │
│  Mutex / thread- │  bookingMutex_ (createBooking)  │  R8 seat double-booking  │
│  safety          │                                 │  se bachao               │
│  RBAC (ordinal)  │  requireRole() + UserRole       │  R5 role permissions     │
└──────────────────┴─────────────────────────────────┴─────────────────────────┘
```

---

## 1. Facade Design Pattern (system ka single entry point)

### 📌 Concept
Facade ek complex subsystem (bahut saari classes/services) ke aage ek **patla, saaf
interface** rakh deta hai — client ko andar ke moving parts se matlab hi nahi rehta.

### PATTERN KE ROLES
- **Facade** → [`AirlineManagementSystem`](./core/AirlineManagementSystem.h)
- **Subsystem** → 5 services + `IPricingStrategy` + ~10 models

### Implementation
Client ([`main.cpp`](./main.cpp)) me se kitni classes dikhti hain? Sirf `AirlineManagementSystem`
(aur models/enums jo wo pass karta hai). Kisi service ka naam tak nahi aata. Client bas:
```cpp
system.assignAircraftToFlight("AI-101", "AC_320"); // andar: SeatInventoryService seats banata
system.createBooking("P1", "AI-101", seats);       // andar: crew-check + pricing + seat-lock + mutex
system.payForBooking(bk);                          // andar: PaymentService + NotificationService
```

### ⭐ KYUN? (Why Facade)
> Facade khud **saare data ka maalik** hai (flights, seats, bookings, payments...) aur
> services ko wo data pass karta hai. Isse services **stateless** reh paati hain (single
> source of truth ek jagah). Client aur subsystem poore decoupled — kal ko koi service
> badle, `main.cpp` ka ek line nahi badlega.

---

## 2. Strategy Design Pattern (pricing)

### 📌 Concept
Ek algorithm ko interface ke peeche rakh do taaki runtime pe swap ho sake, aur naye
algorithm bina purana code chhue add ho sakein.

### PATTERN KE ROLES
- **Strategy interface** → [`IPricingStrategy`](./strategies/IPricingStrategy.h) (`calculateSeatFare`)
- **Concrete Strategy** → [`SeatClassPricingStrategy`](./strategies/SeatClassPricingStrategy.h) (class-wise multiplier)
- **Context** → `AirlineManagementSystem` — `shared_ptr<IPricingStrategy>` hold karta hai

### Implementation
Constructor me default strategy inject hoti hai:
```cpp
AirlineManagementSystem() : pricingStrategy_(std::make_shared<SeatClassPricingStrategy>()) {}
```
Booking ke waqt facade sirf `pricingStrategy_->calculateSeatFare(seat)` bulata hai —
usse pata nahi andar kaunsa formula hai (Economy x1.0, Premium x1.4, Business x2.5).

### ⭐ KYUN? (Why Strategy)
> Aaj simple class-wise pricing hai. Kal ko **DynamicPricingStrategy** (demand/time ke
> hisaab se surge), ya **FestiveDiscountStrategy** chahiye? Bas nayi class banao jo
> `IPricingStrategy` implement kare, aur inject kar do. Seat/Booking/Facade ka ek line
> nahi badlega. **Open/Closed Principle.**

---

## 3. Service Layer / Single Responsibility (SRP)

### 📌 Concept
Business logic ko chhoti, **ek-kaam-wali** services me tod do. Facade orchestrate karta
hai; har service apna ek domain sambhalti hai.

| Service | Ek hi zimmedari | Requirement |
|---------|-----------------|-------------|
| [`FlightSearchService`](./services/FlightSearchService.h) | source/dest/day pe SCHEDULED flights | #1 |
| [`SeatInventoryService`](./services/SeatInventoryService.h) | seats generate / available / lookup | #2, #3 |
| [`CrewAssignmentService`](./services/CrewAssignmentService.h) | crew assign + minimum-crew check | #3 |
| [`PaymentService`](./services/PaymentService.h) | payment process + refund | #2, #6 |
| [`NotificationService`](./services/NotificationService.h) | passenger ko khabar | #9 |

### ⭐ KYUN? (Why Service Layer)
> Har service **stateless static helper** hai — facade data deta hai, wo natija laut deti
> hai (koi hidden state nahi). Isse har service ko **akele test karna trivial** hai. Aur
> ek jagah ka bug doosri jagah nahi failता. Facade patla rehta hai, services reusable.

---

## 4. Concurrency — Mutex Critical Section (R8) ⭐

### 📌 Concept
Jab kai requests ek saath aa sakti hain aur ek hi shared resource (seat) ko chhu rahi
hain, to us critical section ko **ek waqt me ek hi thread** chalaye — warna race condition.

### Implementation
[`createBooking`](./core/AirlineManagementSystem.h) (aur pay/cancel/change) ke shuru me:
```cpp
std::lock_guard<std::mutex> lock(bookingMutex_);   // critical section start
// ... seat check + seat book ...
// scope khatam -> lock apne aap release (RAII)
```

### ⭐ Do zaroori sookshm baatein (interview me chamakti hain):

**(a) Two-phase: "validate-all-then-commit"**
> Pehle SAARI seats check hoti hain (available? isi flight ki?), phir hi koi book hoti hai.
> Agar ek-ek karke book karte aur 3rd seat unavailable nikalti, to pehli 2 already BOOKED
> ho chuki hotin — aadhi-adhoori booking! Pehle poora check, phir poora commit = **atomicity**
> ("ya saari seats, ya ek bhi nahi").

**(b) `createBookingUnlocked` — re-entrant deadlock se bachav**
> `changeFlight()` pehle se `bookingMutex_` pakde hue hota hai. Agar wo seedha `createBooking()`
> bulata (jo phir se lock leta), to **deadlock** — kyunki `std::mutex` re-entrant nahi hai
> (ek thread do baar lock nahi le sakta). Isi liye ek **private lock-free core**
> (`createBookingUnlocked`) banaya jo lock ke andar se safe bulaya jaata hai.
> 📌 Ye "public method locks, private core assumes lock" ek classic concurrency pattern hai.

---

## 5. RBAC — Role-Based Access Control (R5)

### 📌 Concept
Har action ke liye ek minimum permission-level ho, aur system check kare ki current user
ke paas utni power hai ya nahi.

### Implementation
[`UserRole`](./enums/UserRole.h) ordinal hai: `PASSENGER(0) < AIRLINE_STAFF(1) < ADMIN(2)`.
[`requireRole`](./core/AirlineManagementSystem.h) number compare karta hai:
```cpp
if (static_cast<int>(role) < static_cast<int>(minimumRole))
    throw "Insufficient permissions";
```
| Action | Minimum role |
|--------|-------------|
| `scheduleFlight` | ADMIN |
| `assignAircraftToFlight` / `assignCrewToFlight` / `delayFlight` | AIRLINE_STAFF |
| `searchFlights` / `createBooking` / `payForBooking` / `cancelBooking` | koi bhi |

### ⭐ KYUN? (Why ordinal RBAC)
> Ordinal hone se **hierarchy free me** mil jaati hai — ADMIN (2) apne se neeche STAFF (1)
> ke saare kaam bhi kar sakta hai (2 >= 1), bina alag se likhe. ⚠ Isi liye `UserRole` ka
> **order kabhi mat badalna** — poori permission-hierarchy usi order pe tiki hai.

---

## Object Relationships (UML samajhne ke liye)

```
AirlineManagementSystem  ◆──  5 services + NotificationService  (Composition: facade owns)
AirlineManagementSystem  ◇──  IPricingStrategy                  (Aggregation: injected shared_ptr)
AirlineManagementSystem  ◆──  saare data maps                   (Composition: single source of truth)
CompanyPage/Flight       ··▶  SeatInventoryService              (Dependency: static call)
SeatClassPricingStrategy  ─▷  IPricingStrategy                  (Inheritance: implements)
Booking                  ──▶  Seat (seatIds)                    (Association: ids se refer)
```
- `◆──` Composition · `◇──` Aggregation · `──▶` Association · `··▶` Dependency · `─▷` Inheritance

---

## Requirement → Pattern quick map

| Requirement | Pattern / Mechanism |
|-------------|---------------------|
| #1 search flights | **Service Layer** (FlightSearchService) |
| #2 book + pay | Facade + **two-phase** seat commit + PaymentService |
| #3 schedule/assign | **RBAC** (requireRole) + SeatInventory/Crew services |
| #4 passenger + baggage | `Passenger` model + `validateBaggage` (30 kg rule) |
| #5 user roles | **RBAC** (ordinal `UserRole`) |
| #6 cancel + refund | Facade `cancelBooking` (CONFIRMED→REFUNDED) |
| #7 flight change | Facade `changeFlight` + `createBookingUnlocked` |
| #8 concurrent booking | **Mutex** critical section (`bookingMutex_`) |
| #9 pricing/notify pluggable | **Strategy** + **Service Layer** |
| Simple client API | **Facade** |

---

## "Ye pattern kyun NAHI use kiya" (interview me pooche jaate hain)

- **Observer for notifications?** Abhi 3 seedhe notification methods kaafi hain (booking
  confirmed / refund / flight change). Multiple channels (email+sms+push) ek saath chahiye
  honge tab Observer laga denge — abhi **YAGNI** (zaroorat se pehle mat banao).
- **State pattern for Booking/Flight lifecycle?** Statuses simple hain aur transitions
  facade me ek jagah handle ho jaate hain. Agar transitions bahut badh jaayein (har status
  pe alag behavior), tab State pattern justify hota. Abhi enum + guards saaf aur kaafi.
- **Singleton for the system?** `main()` me ek `AirlineManagementSystem system;` kaafi hai.
  Singleton global state deta jo testing/concurrency ko mushkil karta — zaroorat na ho to mat lagao.
- **Recursive mutex (`std::recursive_mutex`) se deadlock solve kyun nahi kiya?** Wo chalta,
  par recursive mutex aksar "design smell" hota hai (lock kaun pakad raha hai, track karna
  mushkil). Saaf hal: **lock-free private core** (`createBookingUnlocked`) — zyada explicit aur samajhne layak.
```

# IRCTC LLD — Design Diagrams

> Codebase padh ke banaya. Is LLD ki do khaas baatein: **segment-based seat booking**
> (ek seat ek run me alag-alag logon ko de sakte hain, jab tak safar overlap na kare)
> aur **per-run locking** (thread-safe booking).

---

## 1. Class Diagram

```mermaid
classDiagram
    direction LR

    class IRCTCSystem {
        <<Facade>>
        -TrainCatalogService catalog_
        -SeatInventoryService inventory_
        -BookingService bookingService_
        -Map users_
        -int bookingCounter_
        +registerUser(name) userId
        +addTrainRun(Train, coachSeatCount)
        +searchByRoute(src, dst, date)
        +getAvailableSeatCount(...)
        +bookTicket(userId, trainNo, date, src, dst, seat)
        +cancelTicket(bookingId, userId)
        +getBooking(bookingId) TicketBooking
    }

    class TrainCatalogService {
        -Map trainsByRunKey_
        -Map runsByDate_
        +addTrain(Train)
        +getTrainRun(trainNo, date) Train
        +listRunsOnDate(date) Train[]
    }

    class TrainSearchService {
        <<static / stateless>>
        +searchByRoute(catalog, src, dst, date)$ Train[]
        +searchByTrainNumber(catalog, trainNo, date)$ Train*
    }

    class SeatInventoryService {
        -Map seatsByRun_
        +initializeCoach(runKey, seatCount)
        +getSeats(runKey) string[]
    }

    class SeatAllocationService {
        <<static / stateless>>
        +isSeatFreeForSegment(ledger, fromIdx, toIdx)$ bool
        +countAvailableSeats(...)$ int
        +listAvailableSeatIds(...)$ string[]
        +pickFirstAvailableSeat(...)$ string
    }

    class BookingService {
        -SeatInventoryService* inventory_
        -Map bookingsById_
        -Map ledgerByRun_
        -Map runMutexes_
        -mutex registryMutex_
        -mutex bookingsMutex_
        +registerRun(runKey)
        +bookTicket(...) TicketBooking
        +cancelTicket(bookingId, userId)
        +getBooking(bookingId) TicketBooking
    }

    class BookingFactory {
        <<static>>
        +createConfirmed(...)$ TicketBooking
    }

    class Train {
        <<struct>>
        +string trainNumber
        +string runDate
        +string[] stations
        +stationIndex(code) int
        +servesSegment(src, dst) bool
        +runKey() string
    }

    class TicketBooking {
        <<struct>>
        +string bookingId
        +string userId
        +string seatId
        +int fromStationIdx
        +int toStationIdx
        +BookingStatus status
    }

    class SegmentLedger {
        <<map: seatId to Booking[]>>
    }

    class User {
        <<struct>>
        +string userId
        +string name
    }

    class BookingStatus {
        <<enumeration>>
        CONFIRMED
        CANCELLED
    }

    IRCTCSystem *-- TrainCatalogService
    IRCTCSystem *-- SeatInventoryService
    IRCTCSystem *-- BookingService
    IRCTCSystem *-- User : registers
    BookingService o-- SeatInventoryService : uses (ptr)
    BookingService *-- SegmentLedger : per run
    BookingService *-- TicketBooking : by id
    BookingService ..> BookingFactory : creates via
    BookingService ..> SeatAllocationService : checks with
    SegmentLedger o-- TicketBooking : seat to bookings
    IRCTCSystem ..> TrainSearchService : delegates
    TrainSearchService ..> TrainCatalogService : reads
    TrainCatalogService *-- Train
    TicketBooking --> BookingStatus
    BookingFactory ..> TicketBooking : builds
```

---

## 2. ⭐ Core idea — Segment-based seat sharing

Ek train A→B→C→D chalti hai. Ek hi seat `S1` do logon ko di ja sakti hai agar unke
safar **overlap na karein**:

```mermaid
flowchart LR
    subgraph Seat S1 ledger
    direction LR
    A[A] --- B[B] --- C[C] --- D[D]
    end
    P1["Passenger 1: A to C  (idx 0 to 2)"]:::p1
    P2["Passenger 2: C to D  (idx 2 to 3)"]:::p2
    classDef p1 fill:#2563eb,color:#fff
    classDef p2 fill:#16a34a,color:#fff
```

**Overlap check** (`SegmentUtils::segmentsOverlap`) half-open interval `[from, to)` use
karta hai:

```
overlap(fromA, toA, fromB, toB) = (fromA < toB) && (fromB < toA)
```

| P1 segment | P2 segment | Overlap? | Same seat de sakte? |
|---|---|---|---|
| A→C `[0,2)` | C→D `[2,3)` | `0<3 && 2<2` = **false** | ✅ haan (C pe P1 utar gaya) |
| A→C `[0,2)` | B→D `[1,3)` | `0<3 && 1<2` = **true** | ❌ nahi (B–C dono chahiye) |

Isi wajah se seat ka data ek single `bool isBooked` nahi, balki **bookings ki list**
(`SegmentLedger[seatId] = vector<TicketBooking>`) hai — har booking apna segment rakhti hai.

---

## 3. Sequence — bookTicket (thread-safe)

```mermaid
sequenceDiagram
    autonumber
    actor User
    participant Sys as IRCTCSystem
    participant Cat as TrainCatalogService
    participant BS as BookingService
    participant Alloc as SeatAllocationService
    participant Fac as BookingFactory

    User->>Sys: bookTicket(userId, trainNo, date, src, dst, seat)
    Sys->>Sys: validateUser(userId)
    Sys->>Cat: getTrainRun(trainNo, date)
    Cat-->>Sys: Train
    Sys->>BS: bookTicket(train, ...)

    Note over BS: lock_guard(runMutex[runKey]) 🔒<br/>sirf IS run ka lock — baaki trains free
    alt seat diya nahi
        BS->>Alloc: pickFirstAvailableSeat(train, seats, ledger, src, dst)
        Alloc-->>BS: seatId ya ""
    else seat diya hai
        BS->>Alloc: isSeatFreeForSegment(seatLedger, fromIdx, toIdx)
        Alloc-->>BS: true/false
    end
    alt koi seat nahi
        BS-->>User: throw "no seats available"
    else seat mil gaya
        BS->>Fac: createConfirmed(...)
        Fac-->>BS: TicketBooking (PNR)
        BS->>BS: ledger[seatId].push_back(booking)
    end
    Note over BS: run lock chhoda 🔓
    Note over BS: lock_guard(bookingsMutex) 🔒<br/>bookingsById_[id] = booking
    BS-->>Sys: TicketBooking
    Sys-->>User: PNR
```

---

## 4. ⭐ Concurrency design — teen alag locks

```mermaid
flowchart TD
    subgraph Locking strategy
    R["registryMutex_<br/>(runs register karte waqt)"]
    RM["runMutexes_ [runKey]<br/>(per-train-run lock)"]
    BM["bookingsMutex_<br/>(bookingsById_ map ke liye)"]
    end
    note1["Alag-alag trains ki booking<br/>ek saath chal sakti hai<br/>(fine-grained locking)"]
    note2["Ek hi train ki do booking<br/>serialize hoti hain<br/>(double-allot se bachaav)"]
    RM --> note1
    RM --> note2
```

> **Bug jo fix hua tha:** pehle `runMutexes_[runKey]` (operator `[]`) lock ke andar map ko
> structurally modify kar deta tha — chicken-and-egg race. Fix: run ko setup ke waqt hi
> `registerRun()` se pre-create karo, aur booking me sirf `.at()` se dhoondo (map badalta
> nahi). TSan se verify kiya gaya tha.

---

## 5. Design patterns

| Pattern | Kahan | Kyun |
|---|---|---|
| **Facade** | `IRCTCSystem` | ek darwaza, andar 5 services |
| **Factory** | `BookingFactory` | PNR + booking banane ka ek jagah |
| **Service layer / SRP** | Catalog, Search, Inventory, Allocation, Booking | ek class ek kaam |
| **Stateless helper** | `TrainSearchService`, `SeatAllocationService` | pure functions, `static` |
| **Fine-grained locking** | `runMutexes_` per run | alag trains parallel, same train serialized |
| **Ledger / event-sourcing-lite** | `SegmentLedger` | seat ki state = bookings ki list se derive |

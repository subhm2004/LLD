# Design Patterns Used — IRCTC LLD

Ye ek train-booking system hai jiska highlight **segment-based seat allocation** hai (ek
physical seat, alag-alag non-overlapping route-segments pe alag passengers ko bik sakti hai) +
**concurrent booking safety**. Design ek Facade, ek Factory, ek layered service architecture,
aur per-train-run locking pe khada hai.

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                            PATTERN OVERVIEW                                    │
├──────────────────┬─────────────────────────────────┬─────────────────────────┤
│  Pattern         │  Kahan (class)                  │  Kis liye                │
├──────────────────┼─────────────────────────────────┼─────────────────────────┤
│  Facade          │  IRCTCSystem                    │  system ka single entry  │
│  Factory         │  BookingFactory                 │  TicketBooking banana    │
│  Service Layer   │  Catalog/Search/Inventory/      │  SRP: har concern alag   │
│  (SRP)           │  Allocation/Booking             │                          │
│  Per-run mutex ⭐│  BookingService (lock striping) │  concurrent booking safe │
│  Segment alloc   │  SeatAllocation + SegmentUtils  │  seat reuse (half-open)  │
│  (technique)     │                                 │                          │
└──────────────────┴─────────────────────────────────┴─────────────────────────┘
```

---

## 1. Facade — `IRCTCSystem`

### 📌 Concept
Complex subsystem (5 services + factory) ke aage ek patla, saaf interface.

### Implementation
Client ([`main.cpp`](./main.cpp)) sirf `IRCTCSystem` jaanta hai: `registerUser`, `addTrainRun`,
`searchByRoute`, `bookTicket`, `cancelTicket`. Andar catalog/inventory/allocation/booking ka
orchestration chhupa hai. Facade services ko **wire** bhi karta hai
(`BookingService bookingService_{&inventory_}` — booking ko inventory ka ref deta seats ke liye).

### ⭐ KYUN?
> Client aur subsystem decoupled. Segment-overlap math, per-run locking, do-index search — sab
> hidden. Kal ko koi service badle, `main.cpp` untouched.

---

## 2. Factory — `BookingFactory`

[`BookingFactory::createConfirmed`](./factories/BookingFactory.h) ek fully-filled CONFIRMED
`TicketBooking` banata hai (10 fields + PNR generation ek jagah).
> **KYUN?** Booking object banane ka logic ek jagah — kal ko field add ho (fare, class) ya PNR
> scheme badle, to sirf factory badlegi, har booking-site nahi. **Single point of change.**

---

## 3. Service Layer / SRP

| Service | Ek hi zimmedari | Requirement |
|---------|-----------------|-------------|
| [`TrainCatalogService`](./services/TrainCatalogService.h) | trains store + lookup (do index) | FR-1, FR-2 |
| [`TrainSearchService`](./services/TrainSearchService.h) | route/number se search (query layer) | FR-1, FR-2 |
| [`SeatInventoryService`](./services/SeatInventoryService.h) | run ki seat list ("S1".."Sn") | setup |
| [`SeatAllocationService`](./services/SeatAllocationService.h) | segment ke liye seat free/available | FR-3, FR-5, FR-6 |
| [`BookingService`](./services/BookingService.h) | book/cancel + concurrency | FR-4, FR-7, FR-8 |

> **KYUN?** Har concern apni class me — test karna, badalna, samajhna aasaan. Storage (catalog)
> aur query (search) alag; seat existence (inventory) aur seat bookings (allocation/ledger) alag.

---

## 4. Technique: Segment-based seat allocation ⭐ (is system ka highlight)

### 📌 Idea
Ek physical seat ek route ke alag-alag **non-overlapping** legs pe alag passengers ko bik sakti
hai. Isi liye ek seat "booked" ya "free" nahi — wo ek SEGMENT ke liye booked/free hoti hai.

### Kaise? — half-open intervals `[from, to)`
Route ordered hai, har station ka index. Ek journey = `[fromIdx, toIdx)`.
[`SegmentUtils::segmentsOverlap`](./utils/SegmentUtils.h): `fA < tB && fB < tA`.
```
S1 pe:  Alice BOM(0)->BRC(2) = [0,2)      Bob RTM(3)->NDLS(6) = [3,6)
        [0,2) vs [3,6): 0<6 haan, 3<2 NAHI -> overlap FALSE -> DONO S1 pe! ✅
        Carol ST(1)->KOTA(4) = [1,4) vs [0,2): 1<2 haan, 0<4 haan -> overlap -> REJECT ❌
```
> **⭐ Half-open (`to` exclude) kyun?** BRC pe ek passenger UTARTA hai, doosra CHADHTA hai — ye
> conflict nahi. `[0,2)` aur `[2,5)` touch karte hain par overlap nahi. Closed `[0,2]` hota to
> galat conflict banta. Ye is problem ka sabse nazuk detail hai. `CANCELLED` bookings overlap
> check me skip hoti hain -> cancel = segment fir free.

---

## 5. Concurrency: Per-train-run mutex (lock striping) ⭐ + BUG FIX

### 📌 Design (NFR-2)
Har train run (`runKey = number|date`) ka apna mutex. Ek run ki bookings serialize (fair "1 seat
= 1 winner"), par ALAG runs parallel — poore system ko ek lock se nahi atkaate.

### 🐛 Bug jo tha (ThreadSanitizer se pakda) + ✅ Fix
```
PROBLEM (chicken-and-egg): per-run mutex ek MAP me tha (runMutexes_). Mutex NIKAALNE ke liye
   runMutexes_[runKey]  -> operator[] KHUD MAP MODIFY (insert) karta hai!
   8 threads ek saath -> hash-table structure pe DATA RACE (lock lene se PEHLE hi race).
   TSan trace: BookingService::mutexForRun -> unordered_map::operator[] -> __emplace...  💥
```
**Fix (do hisse):**
1. **Pre-create** per-run state (`registerRun`) SETUP-time pe (single-threaded, jab train add
   hoti — `IRCTCSystem::addTrainRun` se). Ab booking me `runMutexes_`/`ledgerByRun_` me koi
   INSERT nahi — sirf `.at()` lookup (concurrent const reads safe).
2. Global `bookingsById_` index ko alag `bookingsMutex_` se guard.

**⚠ Deadlock avoid:** run-lock aur bookings-lock kabhi EK SAATH nahi pakde jaate. `bookTicket`:
pehle run-lock (ledger), phir alag se bookings-lock (index). `cancelTicket`: pehle bookings-lock,
phir alag se run-lock. Sequential (nested nahi) — koi lock-ordering cycle nahi.

**Verified:** ThreadSanitizer **5 runs clean** (pehle har run me race), demo "1 success / 7 rejected".

---

## Object Relationships (UML samajhne ke liye)

```
IRCTCSystem     ◆──  5 services              (Composition: facade owns + wires)
BookingService  ──▶  SeatInventoryService    (Association: pointer, seats ke liye)
BookingService  ··▶  BookingFactory          (Dependency: static call)
BookingService  ··▶  SeatAllocationService   (Dependency: static call)
BookingService  ◆──  runMutexes_/ledgerByRun_/bookingsById_  (Composition: owns per-run state)
SeatAllocation  ··▶  SegmentUtils            (Dependency: overlap math)
SegmentLedger   ─▷   unordered_map           (Inheritance: named map type)
```
- `◆──` Composition · `──▶` Association · `··▶` Dependency · `─▷` Inheritance

---

## Requirement → Pattern/Technique quick map

| Requirement | Kaise pura hua |
|-------------|----------------|
| FR-1 search by route+date | `TrainSearchService::searchByRoute` |
| FR-2 search by number+date | `TrainSearchService::searchByTrainNumber` |
| FR-3 available seat count | `SeatAllocationService::countAvailableSeats` |
| FR-4 book (auto/preferred) | `BookingService::bookTicket` + `BookingFactory` |
| FR-5 reject overlapping segment | `SegmentUtils::segmentsOverlap` |
| FR-6 non-overlapping seat reuse | half-open interval check |
| FR-7 cancel | `BookingService::cancelTicket` (status CANCELLED) |
| FR-8 thread-safe under load | **per-run mutex** (+ race fix) |
| NFR-2 per-run serialized booking | lock striping by `runKey` |

> **Cleanup + fix notes:** (1) 🧹 `main.cpp` me `#include <bits/stdc++.h>` tha (non-portable) —
> hata diya, targeted includes. (2) 🐛 **TSan-verified concurrency race fixed** (upar #5). (3)
> `getBooking` ko **by-value** return kiya (const ref nahi) — thread-safety ke liye copy lauta na
> zaroori (lock chhodne ke baad map-element ref invalid ho sakta).

---

## "Ye pattern kyun NAHI use kiya" (interview me pooche jaate hain)

- **Strategy for seat allocation?** Abhi ek hi policy (first-available / preferred). Kal ko
  window/aisle preference, coach preference chahiye to `IAllocationStrategy` add karte. Abhi YAGNI.
- **Single global mutex (coarse-grained)?** Simple hota, par saari trains ek doosre ka wait
  karti — koi parallelism nahi. Per-run striping se alag trains parallel book hoti (NFR-2). Isliye striping.
- **`shared_mutex` (parallel reads)?** Availability queries read-heavy hain — read lock se parallel
  ho sakte. Par booking (write) common hai aur queries demo me single-threaded, isliye abhi simple
  exclusive lock. Extension.
- **Observer for notifications (ticket confirmed/cancelled SMS)?** Real IRCTC me hota, par is
  scope me out (payment/notifications out of scope). Observer add karna trivial hoga.
- **Interval tree for overlap check?** Abhi linear scan (seat pe kuch hi bookings). Ek seat pe
  bahut segments hon to interval tree O(log n) deta. Interview-scale pe linear kaafi + simple.
```

# Design Patterns Used — OYO Hotel Booking LLD

Ye ek hotel booking system hai jiske highlights: **runtime-switchable pricing** (Strategy),
**date-range room availability** (ek room alag non-overlapping dates pe alag guests ko), aur
ek saaf **booking lifecycle state machine**. Design ek Facade, Strategy, layered service
architecture, aur half-open date-overlap technique pe khada hai.

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                            PATTERN OVERVIEW                                    │
├──────────────────┬─────────────────────────────────┬─────────────────────────┤
│  Pattern         │  Kahan (class)                  │  Kis liye                │
├──────────────────┼─────────────────────────────────┼─────────────────────────┤
│  Facade          │  OYOHotelBookingSystem          │  system ka single entry  │
│  Strategy ⭐     │  IPricingStrategy + 2 impls     │  pricing swap (runtime)  │
│  Service Layer   │  Availability/Pricing/          │  SRP: har concern alag   │
│  (SRP)           │  Notification                   │                          │
│  State machine   │  BookingStatus lifecycle        │  valid workflow enforce  │
│  Date-overlap    │  AvailabilityService (half-open)│  room segment reuse      │
│  (technique)     │                                 │                          │
└──────────────────┴─────────────────────────────────┴─────────────────────────┘
```

---

## 1. Facade — `OYOHotelBookingSystem`

### 📌 Concept
Complex subsystem (3 services + strategy + 4 data stores) ke aage ek patla interface.

### Implementation
Client ([`main.cpp`](./main.cpp)) sirf `OYOHotelBookingSystem` jaanta hai: `registerGuest`,
`addHotel/addRoom`, `searchHotelsByCity`, `getAvailableRooms`, `createBooking`, `cancelBooking`,
`checkIn/checkOut`. Andar availability-math, pricing-strategy, notification — sab hidden.

### ⭐ KYUN?
> Client aur subsystem decoupled. Facade khud data OWN karta hai aur services ko orchestrate.
> `createBooking()` ke peeche "validate → availability check → price compute → confirm → notify"
> ka poora flow chhupa hai — client ko sirf "room book karo" chahiye.

---

## 2. Strategy — Pricing ⭐ (runtime-switchable)

### 📌 Concept
Pricing algorithm ko interface ke peeche rakho taaki runtime pe swap ho sake.

### PATTERN KE ROLES
- **Strategy interface** → [`IPricingStrategy`](./strategies/IPricingStrategy.h) (`calculateTotal`)
- **Concrete strategies** → [`StandardPricingStrategy`](./strategies/StandardPricingStrategy.h) (flat), [`WeekendSurchargePricingStrategy`](./strategies/WeekendSurchargePricingStrategy.h) (Sat/Sun 1.25x)
- **Context** → [`PricingService`](./services/PricingService.h) — `unique_ptr<IPricingStrategy>` hold karta

### Implementation
```cpp
// default: flat rate
system.createBooking("G1", "R102", 10, 12);   // 1800 x 2 = 3600
system.useWeekendPricing();                     // ⭐ RUNTIME swap
system.createBooking("G2", "R101", 12, 15);    // 1200 + 1500 + 1500 = 4200 (weekend surge)
```

### ⭐ KYUN? (Why Strategy)
> Naya pricing (SeasonalStrategy, LoyaltyDiscountStrategy)? Nayi class banao jo `IPricingStrategy`
> implement kare, inject kar do — `PricingService`, booking, facade untouched. Aur `useWeekendPricing()`
> se chalte-chalte formula badal jaata. **Open/Closed Principle.** (WeekendStrategy multiplier bhi
> parameterizable hai — ek class, alag surge levels.)

---

## 3. Service Layer / SRP

| Service | Ek hi zimmedari |
|---------|-----------------|
| [`AvailabilityService`](./services/AvailabilityService.h) | date-range room availability (overlap check) |
| [`PricingService`](./services/PricingService.h) | pricing (Strategy context) |
| [`NotificationService`](./services/NotificationService.h) | email/SMS guest ko |

> **KYUN?** Har concern apni class me — test/badalna/samajhna aasaan. Notification "kaise" bheje
> (console → real gateway) badle to sirf NotificationService; availability logic badle to sirf
> AvailabilityService. Facade patla rehta.

---

## 4. State Machine — Booking + Room lifecycle

### Booking lifecycle ([`BookingStatus`](./enums/BookingStatus.h))
```
CONFIRMED ── checkIn ──► CHECKED_IN ── checkOut ──► CHECKED_OUT
    │
    └── cancel ──► CANCELLED   (SIRF check-in se PEHLE)
```
Facade har transition guard karta hai: `checkIn` sirf CONFIRMED se, `checkOut` sirf CHECKED_IN se,
`cancel` CHECKED_IN ke baad nahi. Galat transition → exception.
> **KYUN?** Invalid workflow (jaise pehle checkout phir checkin, ya guest aane ke baad cancel) apne
> aap block. State + guards = consistent lifecycle.

---

## 5. Technique: Half-open date-overlap availability ⭐

Ek room alag-alag **non-overlapping** date-ranges pe alag guests ko bik sakta hai (segment reuse,
seat/train jaisa). [`AvailabilityService`](./services/AvailabilityService.h):
```
A=[inA, outA), B=[inB, outB) overlap  <=>  inA < outB  AND  inB < outA
   [10,12) vs [12,14): 10<14 haan, 12<12 NAHI -> overlap FALSE ✅ (dono book ho sakti)
   [10,12) vs [11,13): 10<13 haan, 11<12 haan -> overlap TRUE  ❌ (clash)
```
> **⭐ Half-open (`out` exclude) kyun?** Checkout wale din guest chala jaata, room agle ke liye free.
> Closed `[in, out]` hota to 12 pe checkout + 12 pe checkin "conflict" banta (galat). Ye is problem
> ka sabse nazuk detail. CANCELLED/CHECKED_OUT bookings overlap check me skip → wo range fir free.

---

## Object Relationships (UML samajhne ke liye)

```
OYOHotelBookingSystem  ◆──  PricingService/NotificationService  (Composition: facade owns)
OYOHotelBookingSystem  ◆──  guests/hotels/rooms/bookings maps   (Composition: data store)
OYOHotelBookingSystem  ··▶  AvailabilityService                 (Dependency: static call)
PricingService         ◇──  IPricingStrategy                    (Aggregation: injected unique_ptr)
Standard/Weekend       ─▷   IPricingStrategy                    (Inheritance)
Room                   ──▶  Hotel (hotelId)                     (Association: id se)
Booking                ──▶  Guest/Room (ids)                    (Association)
```
- `◆──` Composition · `◇──` Aggregation (injected) · `··▶` Dependency · `─▷` Inheritance · `──▶` Association

---

## Requirement → Pattern quick map

| Requirement | Pattern/Technique |
|-------------|-------------------|
| single entry point | **Facade** |
| searchHotelsByCity (rating sort) | Facade + sort |
| getAvailableRooms (date-range) | **half-open overlap** (AvailabilityService) |
| createBooking (reserve + price) | Facade + **Strategy** (pricing) |
| standard vs weekend pricing | **Strategy** (runtime swap) |
| cancel / checkIn / checkOut | **State machine** (BookingStatus guards) |
| notifications | **Service Layer** (NotificationService) |

> **Cleanup + notes:**
> 1. 🧹 Har file me `#include <bits/stdc++.h>` tha (non-portable, GCC-only) — hata diya, targeted includes.
> 2. 📝 `RoomStatus` (BOOKED/AVAILABLE) availability ka **authority nahi** — availability date-overlap
>    se decide hoti hai (sirf MAINTENANCE gate). Ye status coarse flag hai jo "asli" haalat se thoda
>    alag ho sakta (harmless — code me note kiya). Ye ek design nuance hai, bug nahi.
> 3. 📝 `searchHotelsByCity`/`getAvailableRooms` const methods `const_cast` se mutable pointers dete
>    hain — design wart (cleaner hota `const Hotel*` return), par safe (caller mutate nahi karta). Documented.

---

## "Ye pattern kyun NAHI use kiya" (interview me pooche jaate hain)

- **Observer for notifications?** Real system me hota (booking confirm → email + SMS + push ek saath,
  subscribers). Abhi 3 seedhe methods kaafi (facade directly call karta). Multiple channels chahiye tab Observer.
- **State pattern (alag class per booking state)?** States 5 hain par transitions simple aur ek jagah
  (facade guards). Full State pattern (CheckedInState class etc.) yahan over-engineering — enum + guards saaf.
- **Factory for booking creation?** BookingFactory ho sakta (jaise IRCTC me tha), par yahan booking
  construction simple hai (facade me inline). Fields badhein to factory extract karna worth it.
- **Interval tree for room availability?** Abhi linear scan (kam bookings). Ek room pe bahut bookings
  hon to interval tree O(log n) deta. Interview-scale pe linear kaafi + simple.
- **Thread-safety (mutex)?** In-memory single-thread scope. Concurrent booking chahiye to per-room/
  per-hotel mutex (IRCTC LLD jaisa lock striping) — natural extension.
```

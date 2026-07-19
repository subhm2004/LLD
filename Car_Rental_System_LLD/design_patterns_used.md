# Car Rental System — Design Patterns Used (Hinglish)

> Car rental LLD jisme image wale **saare 13 requirements** implement hue hain,
> **5 design patterns** ke saath. Neeche har pattern ka role, kahan, kaise —
> plus har requirement code me kahan handle hua.

---

## Ek Nazar Me — Saare Patterns

| # | Pattern | Type | Role | Kahan |
|---|---------|------|------|-------|
| 1 | **Factory** | Creational | Vehicle (type+subtype) banana | [`factories/VehicleFactory.h`](./factories/VehicleFactory.h) |
| 2 | **Decorator** | Structural | Rental cost + equipment/service add-ons | [`decorators/`](./decorators/) |
| 3 | **Strategy** | Behavioral | Base pricing pluggable | [`strategies/`](./strategies/) |
| 4 | **Observer** | Behavioral | Overdue notification | [`observers/`](./observers/) |
| 5 | **Facade** | Structural | CarRentalSystem coordinate | [`core/CarRentalSystem.h`](./core/CarRentalSystem.h) |

---

## Requirements → Code Mapping (pehle ye)

| # | Requirement | Kahan | Pattern |
|---|-------------|-------|---------|
| 1 | Users (customer/receptionist) | `models/User.h` | Inheritance |
| 2 | Vehicle types | `enums/VehicleType.h` | — |
| 3 | Vehicle subtypes | `Car/Van/Motorcycle/Truck.h` | Inheritance + Factory |
| 4 | Reservation records | `models/Booking.h` | — |
| 5 | Rental tracking | `Customer::rentedCount_` | — |
| 6 | Cancellation | `cancelBooking()` | — |
| 7 | Vehicle log | `models/VehicleLog.h` | — |
| 8 | Equipment | `AddOn.h` + Decorator | **Decorator** |
| 9 | Services | `AddOn.h` + Decorator | **Decorator** |
| 10 | Overdue + fines | `calculateFine` + notifier | **Observer** |
| 11 | Search | `services/SearchService.h` | — |
| 12 | Branches | `models/Branch.h` | — |
| 13 | Parking stalls | `Branch::totalStalls_` | — |

---

## 1. Factory — Vehicle banana 🏭

**Kahan:** [`factories/VehicleFactory.h`](./factories/VehicleFactory.h)

**Problem:** 4 vehicle types, har ek ke apne subtypes (car: economy/luxury/...). Client ko concrete Car/Van/Motorcycle/Truck classes se coupled nahi hona chahiye.

**Kaise:** `createVehicle(type, subtype, ...)` — type+subtype string do, sahi concrete vehicle milta:

```cpp
createVehicle(CAR, "LUXURY", ...) -> new Car(..., CarType::LUXURY, ...)
createVehicle(TRUCK, "HIGH_DUTY", ...) -> new Truck(...)
```

Naya vehicle type (Bus) = ek case + nayi subclass. Baaki system untouched. (L9 Factory dekho.)

---

## 2. Decorator — Rental cost with add-ons 🎁

**Kahan:** [`decorators/`](./decorators/) (IRentalCost + BaseRental + AddOnDecorator)

**Problem (req #8/#9):** Booking me equipment (GPS, child seat) + services (driver, wifi) add ho sakte, jo cost BADHATE hain. Har combination ke liye subclass banana = explosion.

**Kaise:** Base cost ko decorators se WRAP karke har add-on cost jodo:

```
BaseRental (Mercedes 3 days = 15000)
   wrapped in AddOnDecorator (GPS +900)       = 15900
      wrapped in AddOnDecorator (Child +450)  = 16350
         wrapped in AddOnDecorator (Driver +4500) = 20850
```

`getCost()` andar se bahar chalta — har layer apna add karta. **Koi bhi combination** possible (sirf GPS, sirf driver, dono, kuch nahi) bina nayi class ke. shared_ptr se safe cleanup. (L13 Decorator dekho.)

**Demo output:** `Base rental (3 days) + [Equipment: GPS] + [Equipment: Child Seat] + [Service: Driver]` = Rs 20850.

---

## 3. Strategy — Base pricing 🔀

**Kahan:** [`strategies/PricingStrategy.h`](./strategies/PricingStrategy.h) + `DailyPricingStrategy`

Base rental amount kaise calculate ho — swap-able. `DailyPricingStrategy` = rate × days. Kal WeekendSurge, Seasonal, LoyaltyDiscount — bas nayi strategy, `PricingService` (context) me inject.

**Strategy vs Decorator ka clean separation:** Strategy BASE ko price karta, Decorator uspe EXTRAS jodta. Do alag concerns, do alag patterns. (L8 Strategy dekho.)

---

## 4. Observer — Overdue notification 📢

**Kahan:** [`observers/IRentalObserver.h`](./observers/IRentalObserver.h) + `CustomerNotifier`

**Problem (req #10):** Vehicle time pe return na ho to customer ko notify karna + fine.

**Kaise:** System overdue detect karta -> saare observers ko notify:

```
returnVehicle (day 10, due day 7) -> 3 days late
   -> notifyOverdue() -> CustomerNotifier.onOverdue()
      -> "Your rental is OVERDUE by 3 days. Fine Rs 27000."
```

Naya notification channel (SMS/email/push) = naya observer, system untouched. (L12 Observer dekho.)

---

## 5. Facade — CarRentalSystem 🚪

**Kahan:** [`core/CarRentalSystem.h`](./core/CarRentalSystem.h)

Client (main) ko simple API: `registerCustomer`, `addVehicle`, `createBooking`, `cancelBooking`, `returnVehicle`, `searchByType`. Andar ka jhamela (branches, factory, decorator cost buildup, strategy pricing, observer notify, vehicle logs, fine calc) sab chhupa.

```
createBooking(...)   // client ko sirf ye dikhta
   ├── customer/vehicle validate
   ├── base cost (PricingStrategy)
   ├── add-on decorators wrap (Decorator)
   ├── vehicle status RESERVED
   ├── rentedCount++ (tracking)
   └── vehicle log entry
```

(L17 Facade dekho.)

---

## Design Highlights (interview me bolne layak) ⭐

1. **Vehicle hierarchy + Factory:** abstract Vehicle → 4 concrete (polymorphic `getSubtype()`). Factory type+subtype se banata. Clean OOP.

2. **Strategy + Decorator together:** base pricing (Strategy) + add-ons (Decorator) — do independent axes, do patterns. Textbook separation.

3. **Fine calculation:** overdue days × dailyRate × 1.5 (penalty). Late return mehnga — real rental rule.

4. **Rental tracking lifecycle:** rentedCount++ on booking, -- on cancel/return. Active rentals ka accurate count.

5. **Vehicle log as audit trail:** har event (reserve/pickup/return/cancel/late) timestamped. Fleet monitoring.

6. **Branch ownership:** har branch apne vehicles ka malik (destructor cleanup). Parking stalls = capacity guard.

---

## Kya Improve Ho Sakta Hai (practice ideas)

- Real dates (abhi integer day numbers)
- Payment gateway integration (abhi cost calculate hota, charge nahi — L23 Payment se connect)
- Reservation conflicts (same vehicle overlapping dates)
- Raw pointers → smart pointers (branch/user ownership)
- More pricing strategies (weekend/seasonal/loyalty)
- Inter-branch vehicle transfer

---

**Related lessons:** [L8 Strategy](../L8%20Strategy_Design_Patterns/README.md) · [L9 Factory](../L9%20Factory_Design_Pattern/notes/00_INDEX.md) · [L12 Observer](../L12%20Observer_Design_Pattern/) · [L13 Decorator](../L13%20Decorator_Design_Pattern/) · [L17 Facade](../L17%20Facade_Design_Pattern/) · [L23 Payment](../L23%20Payment_gateway_system_LLD/)

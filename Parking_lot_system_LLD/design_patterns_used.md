# Parking Lot System — Design Patterns Used (Hinglish)

> Parking lot LLD jisme image wale **saare 11 requirements** implement hue hain,
> **4 design patterns** ke saath. Neeche har pattern ka role, kahan, aur kaise —
> plus har requirement code me kahan handle hua.

---

## Ek Nazar Me — Saare Patterns

| # | Pattern | Type | Role | Kahan |
|---|---------|------|------|-------|
| 1 | **Strategy** | Behavioral | Pricing + Payment channel swap-able | [`strategies/`](./strategies/) + [`payment/`](./payment/) |
| 2 | **Observer** | Behavioral | DisplayBoard auto-updates on change | [`observers/`](./observers/) + [`models/DisplayBoard.h`](./models/DisplayBoard.h) |
| 3 | **Factory** | Creational | Payment processor (machine/agent) banana | [`payment/PaymentProcessorFactory.h`](./payment/PaymentProcessorFactory.h) |
| 4 | **Facade** | Structural | ParkingLot coordinate karta sab | [`core/ParkingLot.h`](./core/ParkingLot.h) |

---

## Requirements → Code Mapping (pehle ye)

| # | Requirement | Kahan implement hua |
|---|-------------|---------------------|
| 1 | Capacity 40,000 | `ParkingLot` `maxCapacity_` |
| 2 | 4 spot types | `enums/SpotType.h` + `ParkingSpot::canFitVehicle()` |
| 3 | Multiple gates | `models/Gate.h` + `addEntryGate/addExitGate` |
| 4 | 4 vehicle types | `enums/VehicleType.h` |
| 5 | Display board | `models/DisplayBoard.h` (Observer) |
| 6 | Max capacity rule | `ParkingLot::isFull()` in park |
| 7 | Full notification | `DisplayBoard::onParkingUpdate(isFull=true)` |
| 8 | Ticket | `models/Ticket.h` |
| 9 | Payment machine/agent | `payment/` (Strategy + Factory) |
| 10 | Hourly rates | `strategies/HourlyPricingStrategy.h` |
| 11 | Payment methods | `enums/PaymentMethod.h` |

---

## 1. Strategy — Do jagah 🔀

### A. Pricing Strategy ([`strategies/`](./strategies/))
Fee calculate karne ka tareeka swap-able:
- `PricingStrategy` interface + `HourlyPricingStrategy` (duration × vehicle-rate)

Kal DailyPricing, WeekendSurge, FlatRate — koi bhi aa sakti hai, ParkingLot untouched. `calculateFee(vehicleType, entry, exit)`.

### B. Payment Processor Strategy ([`payment/`](./payment/))
Exit pe payment kaise process ho (requirement #9):
- `PaymentProcessor` interface
- `AutomatedMachinePaymentProcessor` (self-service machine)
- `AgentPaymentProcessor` (parking agent/banda)

ParkingLot ko sirf `PaymentProcessor*` chahiye — machine hai ya agent, farq nahi. Naya channel (mobile app) = nayi strategy, baaki untouched. (L8 Strategy dekho.)

---

## 2. Observer — DisplayBoard auto-update 📢

**Kahan:** [`observers/IParkingObserver.h`](./observers/IParkingObserver.h) + [`models/DisplayBoard.h`](./models/DisplayBoard.h)

**Problem jo solve hua:** Display board ko har park/unpark ke baad manually update karna padta — easy to forget, tight coupling.

**Kaise:** ParkingLot ek **observable (subject)** hai. DisplayBoard ek **observer**. Har change pe:

```
park/unpark hua -> ParkingLot.notifyObservers()
                      -> DisplayBoard.onParkingUpdate(availability, isFull)
                          -> board apna state update kar leta
                          -> isFull? -> "PARKING FULL" notification (req #7)
```

Board ko manually refresh nahi karna — **event-driven**. Naya observer (mobile app, entrance LED) = bas interface implement, ParkingLot untouched. (L12 Observer dekho.)

**Full notification (req #6/#7):** jab lot full ho, observers ko `isFull=true` milta — DisplayBoard turant "PARKING FULL" dikhata.

---

## 3. Factory — Payment processor banana 🏭

**Kahan:** [`payment/PaymentProcessorFactory.h`](./payment/PaymentProcessorFactory.h)

`PaymentChannel` (AUTOMATED_MACHINE / AGENT) do, sahi `PaymentProcessor` object lo. Client ko concrete class ka naam nahi pata.

**⭐ Factory + Strategy combo:** Factory creation handle karti (kaunsa processor banao), Strategy behavior (kaise pay karo). Common pairing. (L9 Factory dekho.)

---

## 4. Facade — ParkingLot 🚪

**Kahan:** [`core/ParkingLot.h`](./core/ParkingLot.h)

Client (main.cpp) ko sirf simple API: `parkVehicle`, `unparkVehicle`, `addSpot`, `addObserver`. Andar ka jhamela (spots, tickets, pricing, payment, gates, board notifications, capacity rules) sab chhupa.

```
unparkVehicle(ticket, gate, channel, method)   // client ko sirf ye dikhta
   │
   ├── ticket validate
   ├── fee calculate (PricingStrategy)
   ├── payment processor banao (Factory)
   ├── payment process (Strategy)
   ├── spot free karo
   └── observers notify (DisplayBoard update)
```

6 steps ek method me — client ko coordinate nahi karna. (L17 Facade dekho.)

---

## Design Highlights (interview me bolne layak) ⭐

1. **VehicleType vs SpotType — do alag dimensions:** "kya park ho raha" (car/truck/van/moto) vs "kahan park ho sakta" (handicapped/compact/large/moto). Ek vehicle multiple spots me fit — mapping `ParkingSpot::canFitVehicle()` me.

2. **Best-fit allocation:** vehicle ko **tightest compatible spot pehle** (motorcycle → motorcycle spot, phir compact). Warna motorcycle compact spot le leti aur car ke liye jagah waste! Ye classic parking optimization — `ParkingLot::getSpotPreference()`.

3. **Handicapped permit rule:** sirf permit wale HANDICAPPED spot use kar sakte (`Vehicle::hasHandicappedPermit()` check). Aur unke liye handicapped spot preference me sabse pehle (convenient spot ka haq).

4. **Ceiling hourly billing:** 1hr 1min = 2hr charge (part-hour bhi full-hour). Real parking rule — `(seconds + 3599) / 3600`.

5. **Payment: channel × method:** channel (machine/agent) aur method (card/cash/upi) DO alag axes — "machine pe UPI" ya "agent ko cash".

---

## Kya Improve Ho Sakta Hai (practice ideas)

- **Multi-floor support** — abhi single flat lot; floors + floor-wise display
- **Spot reservation** — advance booking (abhi first-come)
- Raw pointers → smart pointers
- **Singleton** ParkingLot (typically single instance)
- Daily-reset logic, real payment gateway integration
- Spot allocation ko O(1) — abhi linear scan (bade lot me slow), per-type available queues rakho

---

**Related lessons:** [L8 Strategy](../L8%20Strategy_Design_Patterns/README.md) · [L9 Factory](../L9%20Factory_Design_Pattern/notes/00_INDEX.md) · [L12 Observer](../L12%20Observer_Design_Pattern/) · [L17 Facade](../L17%20Facade_Design_Pattern/) · [L23 Payment Gateway](../L23%20Payment_gateway_system_LLD/) (fuller payment system)

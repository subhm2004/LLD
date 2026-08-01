# Requirements: Parking Lot System

> Ye requirements image wale "Parking Lot Requirements Simplified" ke hisaab se
> updated hain. Har requirement ke saamne likha hai code me kahan implement hua.

## Functional Requirements

1. **Parking Capacity** — Lot up to **40,000 vehicles** accommodate kare.
   → `ParkingLot` constructor ka `maxCapacity` (default 40000).

2. **Types of Parking Spots** — 4 distinct categories:
   - `HANDICAPPED` — sirf handicapped permit wale vehicles
   - `COMPACT` — chhoti cars/bikes ke liye
   - `LARGE` — bade vehicles (truck/van) ke liye
   - `MOTORCYCLE` — sirf motorcycles
   → `enums/SpotType.h`, fit-mapping `ParkingSpot::canFitVehicle()`.

3. **Entrances and Exits** — multiple entry/exit points (congestion kam).
   → `models/Gate.h`, `ParkingLot::addEntryGate/addExitGate`.

4. **Vehicles Allowed** — Cars, Trucks, Vans, Motorcycles.
   → `enums/VehicleType.h`.

5. **Display Board** — har spot type ki availability dikhaye.
   → `models/DisplayBoard.h` (Observer — auto-updates).

6. **Max Capacity Rule** — full hone pe aur vehicle admit na ho.
   → `ParkingLot::isFull()` check in `parkVehicle()`.

7. **Full Lot Notification** — full hone pe entrance + display board pe notification.
   → `DisplayBoard::onParkingUpdate()` (isFull=true pe "PARKING FULL").

8. **Parking Ticket** — entry pe ticket, exit pe fee calc + pay.
   → `models/Ticket.h`, `parkVehicle()` deta hai.

9. **Payment Options at Exit** — automated machine YA parking agent.
   → `payment/` — `AutomatedMachinePaymentProcessor`, `AgentPaymentProcessor` (Strategy + Factory).

10. **Hourly Rates** — charges duration ke hisaab se, hourly.
    → `strategies/HourlyPricingStrategy.h`.

11. **Payment Methods** — credit/debit card, cash, UPI.
    → `enums/PaymentMethod.h`.

## Non-Functional Requirements

1. Pricing logic pluggable (Strategy pattern). ✅
2. Payment channels pluggable (Strategy + Factory). ✅
3. Display auto-updates on change (Observer pattern). ✅
4. Code modular, naye vehicle/spot/payment types extend karna easy. ✅
5. Basic validations (invalid ticket, full lot, incompatible spot). ✅

## Design Notes

- **Best-fit allocation**: vehicle ko tightest compatible spot pehle (motorcycle
  ko motorcycle spot, phir compact) — spots waste na hon.
- **VehicleType vs SpotType**: do alag dimensions — kya park ho raha vs kahan.

## Assumptions

1. Single lot, single process.
2. Actual payment gateway integration out of scope (demo processors).
3. Reservations/subscriptions out of scope.
4. Daily-reset counters, multi-floor future extension ke liye chhode.

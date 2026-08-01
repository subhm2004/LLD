# Requirements: Car Rental System

> Ye requirements image wale "Car Rental System Requirements Simplified" ke
> hisaab se updated hain. Har requirement ke saamne code me kahan implement hua.

## Functional Requirements (image ke 13 points)

1. **Users** — 2 types: Customer (rent karta) + Receptionist (manage karta).
   → `models/User.h` (User base → Customer/Receptionist), `enums/UserRole.h`.

2. **Vehicle Types** — cars, trucks, vans, motorcycles.
   → `enums/VehicleType.h`, Vehicle hierarchy.

3. **Vehicle Subtypes**:
   - Cars: economy/luxury/standard/compact → `models/Car.h`
   - Vans: passenger/cargo → `models/Van.h`
   - Motorcycles: cruiser/touring/sports → `models/Motorcycle.h`
   - Trucks: light/medium/high-duty → `models/Truck.h`

4. **Reservation Records** — kaun rent kiya + kab issue hui.
   → `models/Booking.h` (customerId, vehicleId, issuedDay).

5. **Rental Tracking** — customer ne kitni gaadiyan rent ki.
   → `Customer::rentedCount_` (booking pe ++/--).

6. **Reservation Cancellation** — customer reservation cancel kar sake.
   → `CarRentalSystem::cancelBooking()`.

7. **Vehicle Log** — har vehicle ki detailed activity log.
   → `models/VehicleLog.h` (har event logged).

8. **Additional Equipment** — ski rack, child seat, GPS.
   → `models/AddOn.h` (EQUIPMENT) + Decorator cost.

9. **Additional Services** — driver, Wi-Fi, roadside assistance.
   → `models/AddOn.h` (SERVICE) + Decorator cost.

10. **Overdue Notifications and Fines** — late return pe notify + fine.
    → `PricingService::calculateFine()` + Observer (`CustomerNotifier`).

11. **Vehicle Search** — type ya model se search.
    → `services/SearchService.h`.

12. **Branch Management** — multiple branches oversee.
    → `models/Branch.h`, `branches_` in system.

13. **Parking Facilities** — har branch me parking stalls.
    → `Branch::totalStalls_` (vehicle add pe stall check).

## Non-Functional Requirements

1. Pricing pluggable (Strategy). ✅
2. Add-ons flexible (Decorator — koi bhi combination). ✅
3. Vehicle creation centralized (Factory). ✅
4. Notifications decoupled (Observer). ✅
5. Simple client API (Facade). ✅
6. Modular, extend karna easy (naye vehicle/addon/pricing types). ✅

## Assumptions

1. Date = integer "day number" (simplification).
2. Payment gateway integration out of scope (cost calculate hota, actual charge nahi).
3. Persistence/DB out of scope (in-memory).
4. Fine rate = 1.5× daily rate per overdue day.

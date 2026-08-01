# Car Rental System LLD — Design Diagrams

> Codebase padh ke banaya. Yeh is repo ka **sabse pattern-rich** LLD hai — Decorator,
> Factory, Strategy, Observer, aur do inheritance hierarchies, sab ek jagah.

---

## 1. Class Diagram — poora structure

```mermaid
classDiagram
    direction TB

    class CarRentalSystem {
        <<Facade>>
        -Map customers_
        -Map receptionists_
        -Branch[] branches_
        -Map bookings_
        -Map vehicleLogs_
        -IRentalObserver[] observers_
        -PricingService pricingService_
        +addVehicle(branchId, type, subtype, ...)
        +createBooking(custId, vehId, start, end, addOns) bookingId
        +cancelBooking(id)
        +pickupVehicle(id, day)
        +returnVehicle(id, actualReturnDay)
        +addObserver(IRentalObserver*)
        -notifyOverdue(...)
    }

    %% ---- Vehicle inheritance ----
    class Vehicle {
        <<abstract>>
        #string id_
        #VehicleType type_
        #double dailyRate_
        #VehicleStatus status_
        +getSubtype()* string
        +describe() string
        +setStatus(VehicleStatus)
    }
    class Car
    class Truck
    class Van
    class Motorcycle

    %% ---- User inheritance ----
    class User {
        <<abstract>>
        #string id_
        #UserRole role_
    }
    class Customer {
        -string licenseNumber_
        -int rentedCount_
        +incrementRentedCount()
        +decrementRentedCount()
    }
    class Receptionist {
        -string employeeId_
    }

    %% ---- Decorator (cost) ----
    class IRentalCost {
        <<interface>>
        +getCost()* double
        +getDescription()* string
    }
    class BaseRental {
        -double baseAmount_
        -int days_
    }
    class AddOnDecorator {
        -shared_ptr~IRentalCost~ inner_
        -AddOn addOn_
    }

    %% ---- Strategy (pricing) ----
    class PricingStrategy {
        <<interface>>
        +calculateBaseAmount(rate, days)* double
    }
    class DailyPricingStrategy
    class PricingService {
        -shared_ptr~PricingStrategy~ strategy_
        +calculateRentAmount(rate, start, end) double
        +calculateFine(rate, due, actual) double
    }

    %% ---- Observer ----
    class IRentalObserver {
        <<interface>>
        +onOverdue(bookingId, name, days, fine)*
    }
    class CustomerNotifier

    %% ---- Factory ----
    class VehicleFactory {
        <<static>>
        +createVehicle(type, subtype, id, model, rate)$ Vehicle*
    }

    class Booking {
        -string bookingId_
        -int startDay_
        -int endDay_
        -int dueDay_
        -double totalAmount_
        -double fine_
        -AddOn[] addOns_
        -BookingStatus status_
    }
    class Branch {
        -int totalStalls_
        -Vehicle[] vehicles_
        +addVehicle(Vehicle*)
        +findVehicleById(id) Vehicle*
    }
    class AddOn {
        -double dailyPrice_
        -AddOnCategory category_
    }
    class VehicleLog

    Vehicle <|-- Car
    Vehicle <|-- Truck
    Vehicle <|-- Van
    Vehicle <|-- Motorcycle
    User <|-- Customer
    User <|-- Receptionist
    IRentalCost <|.. BaseRental
    IRentalCost <|.. AddOnDecorator
    AddOnDecorator o-- IRentalCost : wraps inner_
    PricingStrategy <|.. DailyPricingStrategy
    PricingService o-- PricingStrategy
    IRentalObserver <|.. CustomerNotifier

    CarRentalSystem *-- Branch
    CarRentalSystem *-- Booking
    CarRentalSystem *-- Customer
    CarRentalSystem *-- PricingService
    CarRentalSystem o-- IRentalObserver : notifies
    CarRentalSystem ..> VehicleFactory : builds via
    CarRentalSystem ..> IRentalCost : builds cost chain
    Branch *-- Vehicle : owns (raw ptr)
    Booking *-- AddOn
    VehicleFactory ..> Vehicle : creates
```

---

## 2. ⭐ Decorator Pattern — cost chain kaise banti hai

Base rent ke upar har add-on ek layer chadhaata hai. `createBooking` me:

```cpp
shared_ptr<IRentalCost> cost = make_shared<BaseRental>(baseAmount, days);
for (const AddOn& addOn : addOns)
    cost = make_shared<AddOnDecorator>(cost, addOn, days);   // ⭐ lapetta jaata hai
```

```mermaid
flowchart LR
    A["AddOnDecorator<br/>(Insurance)"] --> B["AddOnDecorator<br/>(GPS)"] --> C["BaseRental<br/>(rate x days)"]
    A -. getCost recurse .-> B -. getCost recurse .-> C
    C -. "500" .-> B
    B -. "500 + GPS" .-> A
    A -. "500 + GPS + Insurance" .-> R([Total])
```

> **Kyun Decorator (na ki Booking me hardcoded fields)?** Add-ons runtime pe kitne bhi ho
> sakte hain, kisi bhi combination me. Har naya add-on ek object, aur `getCost()` recursively
> sab jod deta hai. Booking class ko har naye add-on ke liye badalna nahi padta. **Open/Closed.**

---

## 3. Booking lifecycle — State transitions

```mermaid
stateDiagram-v2
    [*] --> CREATED : new Booking
    CREATED --> CONFIRMED : createBooking() success<br/>(vehicle RESERVED)
    CONFIRMED --> CANCELLED : cancelBooking()<br/>(vehicle AVAILABLE)
    CONFIRMED --> ACTIVE : pickupVehicle()<br/>(vehicle RENTED)
    ACTIVE --> COMPLETED : returnVehicle()<br/>(vehicle AVAILABLE, fine?)
    CANCELLED --> [*]
    COMPLETED --> [*]
```

**Booking status aur Vehicle status saath-saath chalti hain:**

| Booking event | Booking status | Vehicle status |
|---|---|---|
| createBooking | CONFIRMED | RESERVED |
| pickupVehicle | ACTIVE | RENTED |
| returnVehicle | COMPLETED | AVAILABLE |
| cancelBooking | CANCELLED | AVAILABLE |

---

## 4. Sequence — returnVehicle with overdue (Observer trigger)

```mermaid
sequenceDiagram
    autonumber
    actor Reception
    participant Sys as CarRentalSystem
    participant Price as PricingService
    participant Obs as CustomerNotifier

    Reception->>Sys: returnVehicle(bookingId, actualReturnDay)
    Sys->>Price: calculateFine(rate, dueDay, actualReturnDay)
    alt actualReturnDay > dueDay (LATE)
        Price-->>Sys: fine = overdueDays * rate * 1.5
        Sys->>Sys: booking.setFine(fine)
        Sys->>Obs: onOverdue(bookingId, name, days, fine)
        Note over Obs: [Notification -> Customer]<br/>"Rental OVERDUE, fine applied"
    else on time
        Price-->>Sys: fine = 0
    end
    Sys->>Sys: booking = COMPLETED, vehicle = AVAILABLE
    Sys->>Sys: customer.decrementRentedCount()
    Sys-->>Reception: return complete (+ fine agar hai)
```

---

## 5. Design patterns summary

| Pattern | Kahan | Kyun |
|---|---|---|
| **Facade** | `CarRentalSystem` | ek darwaza, poora subsystem chhupa |
| **Factory** | `VehicleFactory` | `type + subtype` string se sahi Vehicle banao |
| **Decorator** | `IRentalCost` → `BaseRental` + `AddOnDecorator` | add-ons ki cost dynamically layer karo |
| **Strategy** | `PricingStrategy` → `DailyPricingStrategy` | pricing formula swappable (weekly/seasonal aa sakte) |
| **Observer** | `IRentalObserver` → `CustomerNotifier` | overdue pe notify (email/SMS observer add ho sakte) |
| **Template inheritance** | `Vehicle` / `User` abstract base | common data + `getSubtype()` polymorphic |
| **Service layer** | `PricingService`, `SearchService` | business logic alag |

> ⚠ **Note:** `Branch` aur `CarRentalSystem` raw pointers (`Vehicle*`, `Customer*`) own karte
> hain aur destructor me `delete` karte hain. Isliye copy/move ka dhyan rakhna padta hai
> (double-free se bachne ke liye) — ideal me `unique_ptr` behtar hota.

# Parking Lot System LLD — Design Diagrams

> Codebase padh ke banaya. Char patterns yahan ek saath: **Strategy** (pricing),
> **Factory** (payment processor), **Observer** (display board), aur **Facade** (ParkingLot).

---

## 1. Class Diagram

```mermaid
classDiagram
    direction TB

    class ParkingLot {
        <<Facade>>
        -ParkingSpot[] spots_
        -Map activeTickets_
        -Map vehicleTypeByTicket_
        -Gate[] entryGates_
        -Gate[] exitGates_
        -IParkingObserver[] observers_
        -PricingStrategy* pricingStrategy_
        -int maxCapacity_
        +addSpot(id, type)
        +parkVehicle(Vehicle, entryGateId) ticketId
        +unparkVehicle(ticketId, exitGateId, channel, method) fee
        +addObserver(IParkingObserver*)
        +getAvailability() Map
        -findAvailableSpot(Vehicle) ParkingSpot*
        -notifyObservers()
    }

    class ParkingSpot {
        -int spotId_
        -SpotType spotType_
        -bool occupied_
        -string parkedVehicleNumber_
        +canFitVehicle(Vehicle) bool
        +parkVehicle(number)
        +removeVehicle()
    }

    class Vehicle {
        -string number_
        -VehicleType type_
        -bool handicappedPermit_
    }

    class Ticket {
        -string ticketId_
        -string vehicleNumber_
        -int spotId_
        -long long entryTime_
        -int entryGateId_
    }

    class Gate {
        -int gateId_
        -GateType type_
    }

    %% ---- Strategy: pricing ----
    class PricingStrategy {
        <<interface>>
        +calculateFee(type, entry, exit)* double
    }
    class HourlyPricingStrategy {
        +calculateFee(...) double
    }

    %% ---- Strategy + Factory: payment ----
    class PaymentProcessor {
        <<interface>>
        +processPayment(fee, method)* bool
    }
    class AgentPaymentProcessor
    class AutomatedMachinePaymentProcessor
    class PaymentProcessorFactory {
        <<static>>
        +create(channel)$ PaymentProcessor*
    }

    %% ---- Observer ----
    class IParkingObserver {
        <<interface>>
        +onParkingUpdate(availability, isFull)*
    }
    class DisplayBoard {
        -Map availability_
        -bool isFull_
        +display()
    }

    PricingStrategy <|.. HourlyPricingStrategy
    PaymentProcessor <|.. AgentPaymentProcessor
    PaymentProcessor <|.. AutomatedMachinePaymentProcessor
    IParkingObserver <|.. DisplayBoard
    PaymentProcessorFactory ..> PaymentProcessor : creates

    ParkingLot o-- PricingStrategy : uses (ptr)
    ParkingLot *-- ParkingSpot : owns
    ParkingLot *-- Ticket : active
    ParkingLot *-- Gate
    ParkingLot o-- IParkingObserver : notifies
    ParkingLot ..> PaymentProcessorFactory : pays via
    ParkingSpot ..> Vehicle : canFitVehicle?
    Ticket ..> ParkingSpot : spotId
```

---

## 2. ⭐ Spot-fit matching — kaunsa vehicle kis spot me?

`ParkingSpot::canFitVehicle` decide karta hai. Handicapped spot ke liye permit chahiye,
warna vehicle type dekhta hai:

```mermaid
flowchart TD
    A[canFitVehicle Vehicle] --> B{spot occupied?}
    B -- haan --> X[false]
    B -- nahi --> C{spot == HANDICAPPED?}
    C -- haan --> D{vehicle ke paas permit?}
    D -- haan --> Y[true]
    D -- nahi --> X
    C -- nahi --> E{spot type?}
    E -- MOTORCYCLE --> F{vehicle == MOTORCYCLE?}
    E -- COMPACT --> G{vehicle == CAR ya MOTORCYCLE?}
    E -- LARGE --> Y
    F -- haan --> Y
    F -- nahi --> X
    G -- haan --> Y
    G -- nahi --> X
```

> **LARGE spot sab ko fit karta hai** (truck, van, car, bike). MOTORCYCLE spot sirf bike.
> COMPACT me car ya bike. Isliye `findAvailableSpot` ek **preference order** (`getSpotPreference`)
> follow karta hai — chhota vehicle pehle chhote spot me daalne ki koshish, taaki LARGE spot
> trucks ke liye bache rahein.

---

## 3. Sequence — parkVehicle (entry)

```mermaid
sequenceDiagram
    autonumber
    actor Driver
    participant PL as ParkingLot
    participant Spot as ParkingSpot
    participant Board as DisplayBoard

    Driver->>PL: parkVehicle(vehicle, entryGateId)
    alt parking full
        PL->>Board: notifyObservers() (isFull=true)
        PL-->>Driver: throw "Parking lot is full"
    else spot dhoondo
        PL->>PL: findAvailableSpot(vehicle)<br/>(preference order me)
        PL->>Spot: canFitVehicle(vehicle)
        Spot-->>PL: true
        PL->>Spot: parkVehicle(number)
        PL->>PL: naya Ticket banao (entryTime = now)
        PL->>Board: notifyObservers() (updated availability)
        PL-->>Driver: ticketId
    end
```

---

## 4. Sequence — unparkVehicle (exit + payment)

Yahan **Strategy + Factory dono** ek saath dikhte hain:

```mermaid
sequenceDiagram
    autonumber
    actor Driver
    participant PL as ParkingLot
    participant Price as PricingStrategy
    participant Fac as PaymentProcessorFactory
    participant Pay as PaymentProcessor
    participant Board as DisplayBoard

    Driver->>PL: unparkVehicle(ticketId, exitGate, channel, method)
    PL->>Price: calculateFee(type, entryTime, now)
    Note over Price: hours = ceil(seconds/3600)<br/>rate: bike 20, car 40, van 60, truck 80
    Price-->>PL: fee
    PL->>Fac: create(channel)
    Note over Fac: AGENT -> AgentProcessor<br/>else -> AutomatedMachineProcessor
    Fac-->>PL: PaymentProcessor*
    PL->>Pay: processPayment(fee, method)
    Pay-->>PL: paid = true
    PL->>PL: spot.removeVehicle(), ticket delete
    PL->>Board: notifyObservers()
    PL-->>Driver: fee (receipt)
```

---

## 5. Design patterns summary

| Pattern | Kahan | Kyun |
|---|---|---|
| **Facade** | `ParkingLot` | ek darwaza; spots/tickets/gates/payment andar |
| **Strategy** | `PricingStrategy` → `HourlyPricingStrategy` | pricing formula swappable (flat/tiered/weekend aa sakte) |
| **Strategy** | `PaymentProcessor` → Agent / AutomatedMachine | payment channel ka apna behavior |
| **Factory** | `PaymentProcessorFactory` | `channel` enum → sahi processor, `new` ek jagah |
| **Observer** | `IParkingObserver` → `DisplayBoard` | availability badle to boards auto-update |
| **Service/SRP** | Spot, Ticket, Gate alag models | ek class ek kaam |

> ⚠ **Note:** `ParkingLot` raw pointers (`ParkingSpot*`, `Ticket*`, `Gate*`, `PricingStrategy*`)
> own karta hai aur destructor me `delete` karta hai. `PaymentProcessor` factory se `new` hota
> hai aur use ke turant baad `delete` — short-lived object. `unique_ptr` se ye safer ho sakta hai.

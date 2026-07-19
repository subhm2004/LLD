# Parking Lot System LLD (C++)

Low-Level Design of a parking lot system — image wale "Parking Lot Requirements
Simplified" ke **saare 11 requirements** implement karta hai, 4 design patterns
ke saath.

## Features (requirements se mapped)

- **4 spot types** — Handicapped / Compact / Large / Motorcycle
- **4 vehicle types** — Car / Truck / Van / Motorcycle
- **Capacity + Max rule** — up to 40,000, full hone pe admit band
- **Display Board** — per-type availability + full notification (Observer)
- **Multiple gates** — multiple entry/exit points
- **Ticket** — entry pe, exit pe fee + payment ke liye
- **Payment** — automated machine YA agent (Strategy+Factory), card/cash/upi
- **Hourly pricing** — duration × vehicle-type rate (Strategy)
- **Best-fit allocation** — tightest compatible spot pehle

## Design Patterns

Detail: [`design_patterns_used.md`](./design_patterns_used.md)

- **Strategy** — PricingStrategy (fee) + PaymentProcessor (machine/agent)
- **Observer** — DisplayBoard ← ParkingLot (auto-update on change)
- **Factory** — PaymentProcessorFactory (channel se processor)
- **Facade** — ParkingLot (client ko simple API, andar sab coordinate)

## Structure

```
Parking_lot_system_LLD/
├── main.cpp                       # full demo (all requirements)
├── requirements.md                # 11 requirements + mapping
├── design_patterns_used.md        # patterns detail
├── enums/
│   ├── VehicleType.h              # Car/Truck/Van/Motorcycle
│   ├── SpotType.h                 # Handicapped/Compact/Large/Motorcycle
│   ├── PaymentMethod.h            # Card/Cash/UPI
│   └── PaymentChannel.h           # Machine/Agent
├── models/
│   ├── Vehicle.h                  # + handicapped permit
│   ├── ParkingSpot.h              # SpotType + fit-mapping
│   ├── Ticket.h                   # entry ticket
│   ├── Gate.h                     # entry/exit gates
│   └── DisplayBoard.h             # Observer (availability + full)
├── observers/
│   └── IParkingObserver.h         # Observer interface
├── payment/
│   ├── PaymentProcessor.h         # Strategy interface
│   ├── AutomatedMachinePaymentProcessor.h
│   ├── AgentPaymentProcessor.h
│   └── PaymentProcessorFactory.h  # Factory
├── strategies/
│   ├── PricingStrategy.h          # Strategy interface
│   └── HourlyPricingStrategy.h    # hourly rates
└── core/
    └── ParkingLot.h               # Facade + Observable + rules
```

## Run

```bash
cd Parking_lot_system_LLD
g++ -std=c++17 main.cpp -o parking_app
./parking_app
```

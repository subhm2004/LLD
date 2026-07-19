# Car Rental System LLD (C++)

Low-Level Design of a car rental platform — image wale "Car Rental System
Requirements Simplified" ke **saare 13 requirements** implement karta hai,
**5 design patterns** ke saath.

## Features (requirements se mapped)

- **Users** — Customer + Receptionist (User hierarchy)
- **Vehicle types + subtypes** — Car/Van/Motorcycle/Truck, har ek ke subtypes
- **Reservation records + rental tracking** — kaun/kab/kitni gaadiyan
- **Cancellation** — booking cancel
- **Vehicle log** — har vehicle ki activity timeline
- **Equipment + services** — GPS/child-seat/driver add-ons (Decorator)
- **Overdue + fines** — late return pe notification + fine
- **Search** — type ya model se
- **Branches + parking stalls** — multiple locations, parking capacity

## Design Patterns

Detail: [`design_patterns_used.md`](./design_patterns_used.md)

- **Factory** — VehicleFactory (type+subtype se vehicle)
- **Decorator** — rental cost with equipment/service add-ons
- **Strategy** — PricingStrategy (base pricing pluggable)
- **Observer** — overdue notification (CustomerNotifier)
- **Facade** — CarRentalSystem (client ko simple API)

## Structure

```
Car_Rental_System_LLD/
├── main.cpp                       # full demo (13 requirements)
├── requirements.md                # 13 requirements + mapping
├── design_patterns_used.md        # patterns detail
├── enums/
│   ├── VehicleType.h              # Car/Truck/Van/Motorcycle
│   ├── UserRole.h                 # Customer/Receptionist
│   ├── VehicleStatus.h            # Available/Reserved/Rented/Maintenance
│   └── BookingStatus.h            # Created/Confirmed/Active/Completed/Cancelled
├── models/
│   ├── User.h                     # User base + Customer + Receptionist
│   ├── Vehicle.h                  # abstract base
│   ├── Car.h / Van.h / Motorcycle.h / Truck.h   # concrete + subtypes
│   ├── Booking.h                  # reservation record (+ add-ons, fine)
│   ├── AddOn.h                    # equipment/service add-on
│   ├── VehicleLog.h               # activity log
│   └── Branch.h                   # branch + parking stalls
├── factories/
│   └── VehicleFactory.h           # Factory
├── decorators/
│   ├── IRentalCost.h              # Decorator component
│   ├── BaseRental.h               # base cost
│   └── AddOnDecorator.h           # add-on decorator
├── strategies/
│   ├── PricingStrategy.h          # Strategy interface
│   └── DailyPricingStrategy.h     # concrete
├── observers/
│   ├── IRentalObserver.h          # Observer interface
│   └── CustomerNotifier.h         # overdue notifier
├── services/
│   ├── PricingService.h           # Strategy context + fine calc
│   └── SearchService.h            # search by type/model
└── core/
    └── CarRentalSystem.h          # Facade (all requirements)
```

## Run

```bash
cd Car_Rental_System_LLD
g++ -std=c++17 main.cpp -o rental_app
./rental_app
```

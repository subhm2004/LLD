# Amazon Locker Service LLD

Low-level design for **Amazon Hub Locker**: courier deposit → OTP pickup → compartment release.

## Structure

```
Amazon_Locker_Service_LLD/
├── core/AmazonLockerService.h    # Facade
├── models/                       # Package, AccessCode, LockerCompartment, LockerStation
├── enums/                        # LockerSize, CompartmentStatus, PackageStatus, AccessCodeStatus
├── strategies/                   # ICompartmentAllocationStrategy, FirstFitAllocationStrategy
├── services/                     # AccessCodeService, NotificationService
├── main.cpp                      # Runnable scenarios
├── problem_statement.md
├── requirements.md
└── AMAZON_LOCKER_SERVICE_COMPLETE.md
```

## Build & run

```bash
cd Amazon_Locker_Service_LLD
./compile.sh
./amazon_locker_app
```

## Key flows

1. **Deposit** — First-fit compartment by package size → generate 6-digit code → notify customer.
2. **Pickup** — Validate code (active, not expired, attempts OK) → open compartment → mark picked up.

## Interview entities

| Entity | Role |
|--------|------|
| `LockerStation` | Physical locker bank |
| `LockerCompartment` | Single slot (size + status) |
| `Package` | Order bound to compartment |
| `AccessCode` | One-time pickup OTP |
| `AmazonLockerService` | Orchestrator |

See [AMAZON_LOCKER_SERVICE_COMPLETE.md](./AMAZON_LOCKER_SERVICE_COMPLETE.md) for full design notes and Q&A.

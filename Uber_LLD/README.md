# Uber LLD (C++)

Modular low-level design for an in-memory Uber-like ride hailing platform.

## Structure

- `main.cpp`
- `core/UberSystem.h`
- `services/DriverMatchingService.h`
- `services/FareService.h`
- `models/Rider.h`
- `models/Driver.h`
- `models/Ride.h`
- `models/Location.h`
- `enums/RideStatus.h`
- `enums/DriverStatus.h`

## Features

- Rider registration
- Driver onboarding with live location
- Nearest available driver matching
- Ride request flow
- Ride start OTP verification
- Ride lifecycle:
  - accepted
  - started
  - completed
- Distance-based fare calculation
- Payment processing (CASH / UPI / CARD)
- Final fare and payment status visibility

## Run

```bash
g++ -std=c++17 main.cpp -o uber_app
./uber_app
```

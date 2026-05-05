# Ride Sharing App LLD (C++)

This folder contains a Low-Level Design implementation of a ride sharing platform.

## Components

- `Rider`: rider profile entity
- `Driver`: driver profile + availability + location
- `Ride`: ride lifecycle and fare details
- `PricingService`: distance-based fare logic
- `RideSharingService`: request, allocation, start, and completion orchestration

## Features

- Rider and driver registration
- Nearest available driver allocation
- Ride request flow
- Ride state transitions:
  - accepted
  - started
  - completed
- Fare calculation and completion summary

## Structure

- `main.cpp`
- `core/RideSharingService.h`
- `services/PricingService.h`
- `models/`
- `enums/`
- `utils/GeoUtils.h`

## Run

```bash
g++ -std=c++17 main.cpp -o ride_app
./ride_app
```

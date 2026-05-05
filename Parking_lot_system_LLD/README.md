# Parking Lot System LLD (C++)

This folder contains a Low-Level Design implementation of a parking lot system.

## Components

- `Vehicle`: holds vehicle number and type
- `ParkingSpot`: supports type-based occupancy
- `Ticket`: generated at entry with spot and entry time
- `PricingStrategy`: abstraction for fee calculation
- `HourlyPricingStrategy`: hourly fee logic by vehicle type
- `ParkingLot`: orchestrates entry, exit, availability, and billing

## Features

- Type-specific parking spots (Bike/Car/Truck)
- Entry with ticket generation
- Exit with fee computation
- Spot availability tracking
- Strategy pattern for pricing

## Structure

- `main.cpp`
- `core/ParkingLot.h`
- `models/Vehicle.h`
- `models/ParkingSpot.h`
- `models/Ticket.h`
- `strategies/PricingStrategy.h`
- `strategies/HourlyPricingStrategy.h`
- `enums/VehicleType.h`

## Run

```bash
g++ -std=c++17 main.cpp -o parking_app
./parking_app
```

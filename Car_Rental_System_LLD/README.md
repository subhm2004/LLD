# Car Rental System LLD (C++)

Modular low-level design for an in-memory car rental platform.

## Structure

- `main.cpp`
- `core/CarRentalSystem.h`
- `services/PricingService.h`
- `models/Vehicle.h`
- `models/Customer.h`
- `models/Booking.h`
- `enums/VehicleStatus.h`
- `enums/BookingStatus.h`

## Features

- Customer registration
- Vehicle onboarding
- Booking creation with rent amount calculation
- Pickup and return flow
- Vehicle inventory status tracking

## Run

```bash
g++ -std=c++17 main.cpp -o car_rental_app
./car_rental_app
```

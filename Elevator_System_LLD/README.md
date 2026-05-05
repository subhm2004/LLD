# Elevator System LLD (C++)

This folder contains a Low-Level Design implementation of a multi-elevator system.

## Components

- `ElevatorCar`: handles per-elevator state and movement
- `ElevatorScheduler`: chooses best elevator for external requests
- `ElevatorSystemController`: orchestrates requests and simulation ticks
- Request models: `ExternalRequest`, `InternalRequest`

## Features

- Multiple elevators
- External floor requests (UP/DOWN)
- Internal destination requests
- Tick-based movement simulation
- Basic nearest-elevator scheduling

## Structure

- `main.cpp`
- `core/ElevatorSystemController.h`
- `models/ElevatorCar.h`
- `models/Requests.h`
- `scheduler/ElevatorScheduler.h`
- `enums/Direction.h`

## Run

```bash
g++ -std=c++17 main.cpp -o elevator_app
./elevator_app
```

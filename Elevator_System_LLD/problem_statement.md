# Problem Statement: Elevator System

Design a Low-Level Elevator System for a building with multiple floors and multiple elevators.

The system should support:

- External floor requests (UP/DOWN)
- Internal cabin requests (destination floor)
- Elevator assignment using a scheduling strategy
- Movement simulation over time (tick-based progression)

The objective is to produce a clean object-oriented design in C++ that can be enhanced with advanced scheduling, maintenance mode, and safety logic.

## Scope

- In-memory simulation
- Single process
- Focus on LLD and class interactions

## Key Design Objective

Model elevator behavior and scheduling with clear responsibilities between elevator cars, scheduler, and central controller.

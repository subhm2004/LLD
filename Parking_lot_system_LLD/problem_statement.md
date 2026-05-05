# Problem Statement: Parking Lot System

Design a Low-Level Parking Lot System that supports multiple vehicle types and parking spot management.

The system should handle:

- Vehicle entry and parking spot allocation
- Ticket generation at entry
- Vehicle exit and ticket-based un-parking
- Parking fee calculation based on vehicle type and duration
- Spot availability tracking

The objective is to build an object-oriented and extensible C++ design that can later support multi-floor parking, gates, and multiple pricing strategies.

## Scope

- In-memory implementation
- Single parking lot instance
- Focus on LLD and class design

## Key Design Objective

Keep spot allocation, ticketing, and pricing concerns decoupled so features can evolve independently.

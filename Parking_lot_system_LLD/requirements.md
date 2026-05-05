# Requirements: Parking Lot System

## Functional Requirements

1. System supports vehicle types:
   - BIKE
   - CAR
   - TRUCK
2. Parking lot has type-specific spots.
3. On entry:
   - find appropriate available spot
   - park vehicle
   - generate parking ticket
4. On exit:
   - validate ticket
   - calculate fee using pricing strategy
   - free the occupied spot
5. System should print or expose available spot count by vehicle type.

## Non-Functional Requirements

1. Pricing logic should be pluggable (Strategy pattern).
2. Code should be modular and easy to extend for new vehicle/spot types.
3. In-memory operations should be efficient for demo-level scale.
4. Basic validations should exist for invalid tickets and states.

## Assumptions

1. Single lot and single process for now.
2. Payment gateway integration is out of scope.
3. Reservations and subscription parking are out of scope.

# Requirements: Car Rental System

## Functional Requirements

1. System should allow customer registration.
2. System should allow adding vehicles with per-day rental rate.
3. Customer can create booking for available vehicle.
4. Booking should calculate total amount based on date range.
5. Booking should support pickup flow.
6. Booking should support return flow.
7. Vehicle inventory should expose current status.

## Non-Functional Requirements

1. Design should be modular and easy to extend.
2. Data access should be efficient for in-memory use.
3. Business validations should be deterministic.
4. Core components should have clear separation of concerns.

## Assumptions

1. Payment integration is out of scope.
2. Multiple branches/locations are out of scope.
3. Persistence/database is out of scope.
4. Date is represented as integer day number.

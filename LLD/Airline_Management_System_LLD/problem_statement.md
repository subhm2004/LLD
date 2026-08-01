# Problem Statement: Airline Management System

Design a low-level **airline management** system supporting flight operations, bookings, payments, crew, and role-based access.

## Functional requirements (from spec)

1. **Search flights** by source, destination, and date.
2. **Book flights** — select seats and complete payment.
3. **Manage schedules** — flights, aircraft assignment, crew assignment.
4. **Passenger information** — personal details and baggage limits.
5. **User roles** — passenger, airline staff, administrator.
6. **Cancellations & refunds** — release seats and refund confirmed bookings.
7. **Flight changes** — move confirmed booking to another flight/seats.
8. **Concurrent access** — safe seat booking under parallel requests (mutex).
9. **Scalable / extensible** — modular services, strategy pricing, pluggable notifications.

## Non-functional

- In-memory store (interview scope).
- C++17, layered architecture (models / services / facade).
- Thread-safe booking critical section.

## Out of scope

- Real GDS integration, airport slot management globally
- Distributed transactions across data centers

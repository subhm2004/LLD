# Requirements: Movie Ticket Booking System

## Functional Requirements

1. System should allow adding and managing movies.
2. System should allow adding shows for a movie.
3. Each show should have a seat map with seat types.
4. User should be able to select one or more seats for a show.
5. System should validate seat availability before booking.
6. System should create booking with selected seats and total amount.
7. Seat pricing should be calculated based on seat type.
8. System should process payment for booking.
9. On successful payment:
   - booking should be confirmed
   - confirmation notification should be sent

## Non-Functional Requirements

1. Design should be modular and extensible.
2. Pricing, payment, and notification modules should be decoupled.
3. Booking flow should be deterministic and easy to test.
4. In-memory operations should be efficient for interview-scale data.

## Assumptions

1. Authentication and user profile management are out of scope.
2. Real payment gateway behavior is mocked/simulated.
3. Persistence/database integration is out of scope.
4. Concurrency-safe seat locking is out of current scope.

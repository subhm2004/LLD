# Requirements: Ride Sharing App

## Functional Requirements

1. System should allow rider registration.
2. System should allow driver registration with current location.
3. Rider can request ride with pickup and drop location.
4. System should assign nearest available driver.
5. Ride should support state transitions:
   - REQUESTED
   - ACCEPTED
   - STARTED
   - COMPLETED
   - CANCELLED
6. System should calculate fare based on distance.
7. On ride completion, driver should become available again.

## Non-Functional Requirements

1. Driver matching should be efficient for in-memory use.
2. Ride lifecycle logic should be deterministic and validated.
3. Design should be modular for future extensions.
4. Components should follow clear separation of concerns.

## Assumptions

1. Authentication and payments are out of scope.
2. Real-time GPS tracking is simplified to static point updates.
3. Surge pricing and promotions are out of current scope.
4. Persistence/database integration is out of scope.

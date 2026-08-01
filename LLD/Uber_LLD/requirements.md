# Requirements: Uber-Like Ride Hailing System

## Functional Requirements

1. System should allow rider registration.
2. System should allow driver onboarding with current location.
3. Rider should be able to request a ride with pickup and drop location.
4. System should assign nearest available driver.
5. Ride should support lifecycle transitions:
   - REQUESTED
   - ACCEPTED
   - STARTED
   - COMPLETED
   - CANCELLED
6. System should calculate fare based on trip distance.
7. On ride completion, driver should become available again.
8. Ride start should require OTP verification.
9. Ride completion should include payment processing and status.

## Non-Functional Requirements

1. Matching and fare services should be decoupled.
2. Core operations should be efficient for in-memory interview-scale load.
3. Components should have clean separation of concerns.
4. Logic should be deterministic and easy to test.

## Assumptions

1. Authentication is out of scope.
2. Real-time route optimization and traffic are out of scope.
3. Persistence/database integration is out of scope.
4. Single process execution is sufficient for current design.

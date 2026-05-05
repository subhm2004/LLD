# Requirements: Elevator System

## Functional Requirements

1. System should support multiple elevators.
2. User can place external request from any floor with direction:
   - UP
   - DOWN
3. Passenger inside elevator can place internal destination request.
4. Scheduler should assign best available elevator for each external request.
5. Elevator should move floor-by-floor and stop at requested floors.
6. System should expose elevator state:
   - current floor
   - direction
   - idle/busy status
7. System should run simulation in ticks.

## Non-Functional Requirements

1. Design must be extensible for new scheduling algorithms.
2. Components should have single responsibility.
3. Implementation should be deterministic and testable in memory.
4. Input validation for invalid floors and elevator ids.

## Assumptions

1. Door operations and passenger capacity are simplified.
2. Emergency mode and maintenance mode are out of current scope.
3. Time is modeled via discrete ticks, not real-time threads.

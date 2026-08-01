# Requirements: Load Balancer System

## Functional Requirements

1. System should allow onboarding backend servers.
2. System should support health updates for each server (`UP` / `DOWN`).
3. System should route incoming requests to an available server.
4. System should maintain active connection count per server.
5. System should support strategy-based routing.
6. Strategy should be switchable at runtime.

## Non-Functional Requirements

1. Routing operation should be low latency.
2. Design should be extensible for new algorithms.
3. Components should follow separation of concerns.
4. Behavior should be deterministic for interview-scale in-memory simulation.

## Assumptions

1. Actual network calls and reverse proxy integration are out of scope.
2. Health checks are manually simulated.
3. Persistence/database is out of scope.
4. Single process, in-memory execution model.

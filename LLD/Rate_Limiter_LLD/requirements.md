# Requirements: Rate Limiter System

## Functional Requirements

1. System should enforce request limits per `userId`.
2. Limits should depend on user tier (for example `FREE`, `PREMIUM`).
3. System should support multiple rate limiting strategies:
   - Token Bucket
   - Fixed Window
   - Sliding Window Log
4. System should return allow/block decision for each request.
5. System should reject requests when configured limit is exceeded.
6. Design should allow adding new algorithms with minimal changes.

## Non-Functional Requirements

1. Request validation should be low-latency.
2. Implementation should be thread-safe for concurrent requests.
3. Decision accuracy should be deterministic for configured windows.
4. Design should be modular and testable.

## Assumptions

1. In-memory storage is sufficient for current scope.
2. Distributed synchronization across instances is out of scope.
3. Response composition (like HTTP 429 body) is out of scope; only allow/block is handled.
4. User tiers and limits are statically configured at service startup.

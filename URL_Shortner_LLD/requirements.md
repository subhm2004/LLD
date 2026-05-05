# Requirements: URL Shortner

## Functional Requirements

1. User can submit a valid long URL and receive a short URL.
2. Same long URL should return same short URL (dedup behavior).
3. System can resolve short URL to original long URL.
4. System should track number of times each short URL is accessed.
5. System should expose basic URL stats (click count, creation time).
6. System should reject invalid long URL input.

## Non-Functional Requirements

1. Short code generation should be collision-safe for current in-memory scope.
2. Core operations should be O(1) average using hash maps.
3. Design should be modular and easy to extend.
4. Implementation should be deterministic and testable.

## Assumptions

1. Persistence/database is out of scope for now.
2. User authentication and rate limiting are out of scope.
3. Custom alias support is out of current scope.
4. Expiry and deletion policies are out of current scope.

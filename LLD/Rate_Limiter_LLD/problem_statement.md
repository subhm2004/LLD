# Problem Statement: Rate Limiter System

Design an in-memory rate limiting system that controls how many requests a user can make in a time window.

The system should:

1. Identify users by `userId`.
2. Apply limits based on subscription tier (for example, `FREE` vs `PREMIUM`).
3. Support multiple algorithms (Token Bucket, Fixed Window, Sliding Window Log).
4. Return allow/block decision for each request.
5. Be thread-safe under concurrent access.

The design should be extensible so new algorithms and new user tiers can be added without major code changes.

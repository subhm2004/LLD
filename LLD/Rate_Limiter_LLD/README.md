# Rate Limiter LLD (C++)

A modular Low-Level Design implementation of a user-tier based rate limiting system with multiple algorithm strategies.

## Project Structure

- `Main.cpp`
- `service/RateLimiterService.h`
- `factory/RateLimiterFactory.h`
- `limiter/RateLimiter.h`
- `limiter/TokenBucketRateLimiter.h`
- `limiter/FixedWindowRateLimiter.h`
- `limiter/SlidingWindowLogRateLimiter.h`
- `model/User.h`
- `model/RateLimitConfig.h`
- `enums/UserTier.h`
- `enums/RateLimitType.h`
- `rate_limiter_Algo.md`

## Implemented Features

- User-tier aware throttling (`FREE`, `PREMIUM`)
- Multiple rate limiter strategies:
  - Token Bucket
  - Fixed Window
  - Sliding Window Log
- Factory-based strategy creation
- Thread-safe request checks using mutex
- Concurrency test harness in `Main.cpp`
- Custom namespace: `rate_limiter_lld`

## Current Tier Configuration

- `FREE`: Token Bucket, `10 requests / 60 seconds`
- `PREMIUM`: Fixed Window, `100 requests / 60 seconds`

## Design Notes

- `RateLimiter` is an abstract base for algorithm extensibility.
- `RateLimiterFactory` keeps object creation centralized.
- `RateLimiterService` maps user tier -> algorithm instance.
- State is maintained per user id for accurate enforcement.

## Build and Run

```bash
g++ -std=c++17 Main.cpp -o rate_limiter_app
./rate_limiter_app
```

## Suggested Optimizations (Future)

- per-user lock striping to reduce global mutex contention
- lazy cleanup for inactive user entries
- inject clock abstraction for deterministic unit testing
- add Sliding Window Counter implementation

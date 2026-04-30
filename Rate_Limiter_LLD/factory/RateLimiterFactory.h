#ifndef RATE_LIMITER_FACTORY_H
#define RATE_LIMITER_FACTORY_H

#include <memory>
#include <stdexcept>
#include "../enums/RateLimitType.h"
#include "../model/RateLimitConfig.h"
#include "../limiter/RateLimiter.h"
#include "../limiter/TokenBucketRateLimiter.h"
#include "../limiter/FixedWindowRateLimiter.h"
#include "../limiter/SlidingWindowLogRateLimiter.h"

class RateLimiterFactory {
public:
    static std::unique_ptr<RateLimiter> createRateLimiter(RateLimitType algo, RateLimitConfig config) {
        switch (algo) {
            case RateLimitType::TOKEN_BUCKET:
                return std::make_unique<TokenBucketRateLimiter>(config);
            case RateLimitType::FIXED_WINDOW:
                return std::make_unique<FixedWindowRateLimiter>(config);
            case RateLimitType::SLIDING_WINDOW_LOG:
                return std::make_unique<SlidingWindowLogRateLimiter>(config);
            default:
                throw std::invalid_argument("Unknown algorithm");
        }
    }
};

#endif // RATE_LIMITER_FACTORY_H
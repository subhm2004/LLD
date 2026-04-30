#ifndef RATE_LIMITER_H
#define RATE_LIMITER_H

#include <string>
#include "../enums/RateLimitType.h"
#include "../model/RateLimitConfig.h"

class RateLimiter {
protected:
    const RateLimitConfig config;
    const RateLimitType type;

public:
    RateLimiter(RateLimitConfig config, RateLimitType type)
        : config(config), type(type) {}

    virtual ~RateLimiter() = default;
    virtual bool allowRequest(const std::string& userId) = 0;
};

#endif // RATE_LIMITER_H
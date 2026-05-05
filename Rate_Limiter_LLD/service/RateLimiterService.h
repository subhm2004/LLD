#pragma once

#include <unordered_map>
#include <memory>
#include <stdexcept>
#include "../enums/UserTier.h"
#include "../enums/RateLimitType.h"
#include "../limiter/RateLimiter.h"
#include "../factory/RateLimiterFactory.h"
#include "../model/RateLimitConfig.h"
#include "../model/User.h"

namespace rate_limiter_lld {

class RateLimiterService {
private:
    // UserTier -> RateLimiter map (using int key since enum class not hashable by default)
    std::unordered_map<int, std::unique_ptr<RateLimiter>> rateLimiters;

public:
    RateLimiterService() {
        // FREE: TokenBucket - 10 req/60 sec
        rateLimiters[(int)UserTier::FREE] = RateLimiterFactory::createRateLimiter(
            RateLimitType::TOKEN_BUCKET,
            RateLimitConfig(10, 60)
        );

        // PREMIUM: FixedWindow - 100 req/60 sec
        rateLimiters[(int)UserTier::PREMIUM] = RateLimiterFactory::createRateLimiter(
            RateLimitType::FIXED_WINDOW,
            RateLimitConfig(100, 60)
        );
    }

    bool allowRequest(const User& user) {
        auto it = rateLimiters.find((int)user.getTier());
        if (it == rateLimiters.end()) {
            throw std::invalid_argument("No limiter configured for this tier");
        }
        return it->second->allowRequest(user.getUserId());
    }
};

}

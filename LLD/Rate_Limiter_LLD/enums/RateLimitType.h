#ifndef RATE_LIMIT_TYPE_H
#define RATE_LIMIT_TYPE_H

namespace rate_limiter_lld {
enum class RateLimitType {
    TOKEN_BUCKET,
    LEAKY_BUCKET,
    FIXED_WINDOW,
    SLIDING_WINDOW_LOG,
    SLIDING_WINDOW_COUNTER
};
}

#endif
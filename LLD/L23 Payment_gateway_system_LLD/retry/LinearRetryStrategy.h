// ============================================================================
//  retry/LinearRetryStrategy.h — Concrete Strategy #1: FIXED delay
// ----------------------------------------------------------------------------
//  Har retry ke beech SAME wait (jaise har baar 200ms). getDelayMs() attempt
//  ko IGNORE karta hai — hamesha fixedDelayMs. Simple, predictable.
//  Delay pattern: 200, 200, 200... (flat line).
// ============================================================================
#ifndef PAYMENT_GATEWAY_LLD_RETRY_LINEARRETRYSTRATEGY_H
#define PAYMENT_GATEWAY_LLD_RETRY_LINEARRETRYSTRATEGY_H

#include "RetryStrategy.h"

namespace payment_gateway_lld {
class LinearRetryStrategy : public RetryStrategy {
private:
    int maxRetries;
    int fixedDelayMs;

public:
    LinearRetryStrategy(int maxRetries, int fixedDelayMs)
        : maxRetries(maxRetries), fixedDelayMs(fixedDelayMs) {}

    int getMaxRetries() const override { return maxRetries; }
    int getDelayMs(int) const override { return fixedDelayMs; }
    string getName() const override { return "Linear"; }
};
}

#endif

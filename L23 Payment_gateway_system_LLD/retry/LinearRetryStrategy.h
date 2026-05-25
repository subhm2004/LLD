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

// retry/ExponentialBackoffRetryStrategy.h — Concrete: wait time har attempt par
// exponentially badhta hai (base * 2^attempt). RetryStrategy implement karta hai.
#ifndef PAYMENT_GATEWAY_LLD_RETRY_EXPONENTIALBACKOFFRETRYSTRATEGY_H
#define PAYMENT_GATEWAY_LLD_RETRY_EXPONENTIALBACKOFFRETRYSTRATEGY_H

#include "RetryStrategy.h"

namespace payment_gateway_lld {
class ExponentialBackoffRetryStrategy : public RetryStrategy {
private:
    int maxRetries;
    int baseDelayMs;

public:
    ExponentialBackoffRetryStrategy(int maxRetries, int baseDelayMs)
        : maxRetries(maxRetries), baseDelayMs(baseDelayMs) {}

    int getMaxRetries() const override { return maxRetries; }
    int getDelayMs(int attempt) const override {
        if (attempt <= 0) return 0;
        return baseDelayMs * static_cast<int>(pow(2, attempt - 1));
    }
    string getName() const override { return "ExponentialBackoff"; }
};
}

#endif

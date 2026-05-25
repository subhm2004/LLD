#ifndef PAYMENT_GATEWAY_LLD_RETRY_RETRYSTRATEGYFACTORY_H
#define PAYMENT_GATEWAY_LLD_RETRY_RETRYSTRATEGYFACTORY_H

#include "../enums/RetryStrategyType.h"
#include "ExponentialBackoffRetryStrategy.h"
#include "LinearRetryStrategy.h"

namespace payment_gateway_lld {
class RetryStrategyFactory {
public:
    static RetryStrategy *create(RetryStrategyType type, int maxRetries, int delayMs) {
        if (type == RetryStrategyType::EXPONENTIAL_BACKOFF) {
            return new ExponentialBackoffRetryStrategy(maxRetries, delayMs);
        }
        return new LinearRetryStrategy(maxRetries, delayMs);
    }
};
}

#endif

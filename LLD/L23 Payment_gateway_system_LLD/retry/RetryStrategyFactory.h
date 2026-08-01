// ============================================================================
//  retry/RetryStrategyFactory.h — FACTORY (L9): retry strategy banane ki dukaan
// ----------------------------------------------------------------------------
//  RetryStrategyType do, sahi strategy object lo (Linear/ExponentialBackoff)
//  maxRetries + delay config ke saath. GatewayFactory isse call karta hai
//  jab gateway ko proxy me wrap karta (proxy ko ye strategy milti hai).
//  Simple static factory — creation ek jagah, client ko concrete class ka
//  naam nahi pata. Naya retry type = ek case + nayi strategy class.
// ============================================================================
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

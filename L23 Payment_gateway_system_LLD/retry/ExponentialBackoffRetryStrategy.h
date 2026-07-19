// ============================================================================
//  retry/ExponentialBackoffRetryStrategy.h — Concrete Strategy #2: BADHTA delay
// ----------------------------------------------------------------------------
//  Wait time har retry pe DOUBLE hota: base * 2^(attempt-1).
//  base=100 to delays: 100, 200, 400, 800... (exponential curve).
//
//  ⭐ KYUN better (real-world): server down/overloaded ho to bar-bar jaldi
//  retry karna use aur load deta hai. Badhta wait server ko "saans lene"
//  ka time deta — isliye AWS/Google jaise systems exponential backoff
//  use karte hain (aksar "jitter" randomness ke saath). Retry ka gold standard.
// ============================================================================
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

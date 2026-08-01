// ============================================================================
//  retry/RetryStrategy.h — STRATEGY interface (L8): retry ka TIMING
// ----------------------------------------------------------------------------
//  "Fail hone pe kitni baar aur kitni der baad retry karein?" — ye policy
//  swap-able strategy hai. Proxy isse use karta hai (kab-kitna wait karke
//  retry). Do concrete: Linear (fixed wait) vs ExponentialBackoff (badhta wait).
//  Contract: getMaxRetries (kitni baar), getDelayMs (attempt pe kitni der),
//  getName (logging ke liye). Nayi policy (jaise "jittered backoff") = nayi
//  strategy class — proxy untouched.
// ============================================================================
#ifndef PAYMENT_GATEWAY_LLD_RETRY_RETRYSTRATEGY_H
#define PAYMENT_GATEWAY_LLD_RETRY_RETRYSTRATEGY_H

#include <bits/stdc++.h>
using namespace std;

namespace payment_gateway_lld {
class RetryStrategy {
public:
    virtual ~RetryStrategy() {}
    virtual int getMaxRetries() const = 0;
    virtual int getDelayMs(int attempt) const = 0;
    virtual string getName() const = 0;
};
}

#endif

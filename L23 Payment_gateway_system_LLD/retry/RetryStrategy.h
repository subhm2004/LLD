// retry/RetryStrategy.h — STRATEGY interface for retry timing: "kitni der ruko
// before next attempt". Concrete: Linear vs ExponentialBackoff. Proxy isse use karta.
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

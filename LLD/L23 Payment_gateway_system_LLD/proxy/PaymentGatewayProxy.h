// ============================================================================
//  proxy/PaymentGatewayProxy.h — PROXY (L21): real gateway pe RETRY wrap 🔄
// ----------------------------------------------------------------------------
//  Ye proxy real PaymentGateway ko wrap karta hai — SAME interface deta hai
//  (PaymentGateway extend karta), par ek extra kaam add karta: FAILURE par
//  RETRY. Gateway ko retry logic khud nahi rakhna padta (cross-cutting
//  concern bahar rehta).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  RETRY FLOW (proxy ka asli kaam):                                       │
//  │                                                                          │
//  │   for attempt in 0..maxRetries:                                         │
//  │       if attempt > 0:                                                   │
//  │           delay = retryStrategy->getDelayMs(attempt)  [STRATEGY]        │
//  │           sleep(delay)   // linear: fixed | exponential: badhta         │
//  │       result = realGateway->processPayment()  // asli kaam delegate     │
//  │       if result: break   // pass ho gaya, ruk jao                       │
//  │                                                                          │
//  │  Client ko proxy aur real gateway me FARQ nahi (dono PaymentGateway).   │
//  │  Retry KITNI der aur KITNI baar — ye RetryStrategy decide karti hai     │
//  │  (Proxy + Strategy ka combo!). Baaki methods seedha real ko forward.    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ PROXY vs DECORATOR: yahan proxy "behavior add" (retry) kar raha hai jo
//  Decorator jaisa lagta hai — par intent ACCESS CONTROL/management ka hai
//  (real gateway tak controlled access), isliye Proxy. (Interview me ye
//  overlap aata hai — intent se decide karo, structure se nahi.)
//  Nested ownership: proxy real gateway + retry strategy DONO ka malik.
// ============================================================================
#ifndef PAYMENT_GATEWAY_LLD_PROXY_PAYMENTGATEWAYPROXY_H
#define PAYMENT_GATEWAY_LLD_PROXY_PAYMENTGATEWAYPROXY_H

#include <bits/stdc++.h>
#include <chrono>
#include <thread>

#include "../gateways/PaymentGateway.h"
#include "../retry/RetryStrategy.h"

using namespace std;

namespace payment_gateway_lld {
class PaymentGatewayProxy : public PaymentGateway {
private:
    PaymentGateway *realGateway;
    RetryStrategy *retryStrategy;

public:
    PaymentGatewayProxy(PaymentGateway *gateway, RetryStrategy *strategy)
        : realGateway(gateway), retryStrategy(strategy) {}
    ~PaymentGatewayProxy() override {
        delete realGateway;
        delete retryStrategy;
    }

    bool processPayment(PaymentRequest *request) override {
        bool result = false;
        int maxRetries = retryStrategy->getMaxRetries();
        for (int attempt = 0; attempt < maxRetries; ++attempt) {
            if (attempt > 0) {
                int delayMs = retryStrategy->getDelayMs(attempt);
                cout << "[Proxy][" << retryStrategy->getName() << "] Waiting " << delayMs
                     << "ms before attempt " << (attempt + 1) << " for " << request->sender << ".\n";
                this_thread::sleep_for(chrono::milliseconds(delayMs));
                cout << "[Proxy] Retrying payment (attempt " << (attempt + 1) << ") for "
                     << request->sender << ".\n";
            }
            result = realGateway->processPayment(request);
            if (result) break;
        }
        if (!result) {
            cout << "[Proxy] Payment failed after " << maxRetries << " attempts for " << request->sender
                 << ".\n";
        }
        return result;
    }

    bool validatePayment(PaymentRequest *request) override { return realGateway->validatePayment(request); }
    bool initiatePayment(PaymentRequest *request) override { return realGateway->initiatePayment(request); }
    bool confirmPayment(PaymentRequest *request) override { return realGateway->confirmPayment(request); }
};
}

#endif

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

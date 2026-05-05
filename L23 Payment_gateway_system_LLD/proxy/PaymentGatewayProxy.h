#ifndef PAYMENT_GATEWAY_LLD_PROXY_PAYMENTGATEWAYPROXY_H
#define PAYMENT_GATEWAY_LLD_PROXY_PAYMENTGATEWAYPROXY_H

#include <bits/stdc++.h>

#include "../gateways/PaymentGateway.h"

using namespace std;

namespace payment_gateway_lld {
class PaymentGatewayProxy : public PaymentGateway {
private:
    PaymentGateway *realGateway;
    int retries;

public:
    PaymentGatewayProxy(PaymentGateway *gateway, int maxRetries) : realGateway(gateway), retries(maxRetries) {}
    ~PaymentGatewayProxy() override { delete realGateway; }

    bool processPayment(PaymentRequest *request) override {
        bool result = false;
        for (int attempt = 0; attempt < retries; ++attempt) {
            if (attempt > 0) {
                cout << "[Proxy] Retrying payment (attempt " << (attempt + 1) << ") for " << request->sender << ".\n";
            }
            result = realGateway->processPayment(request);
            if (result) break;
        }
        if (!result) {
            cout << "[Proxy] Payment failed after " << retries << " attempts for " << request->sender << ".\n";
        }
        return result;
    }

    bool validatePayment(PaymentRequest *request) override { return realGateway->validatePayment(request); }
    bool initiatePayment(PaymentRequest *request) override { return realGateway->initiatePayment(request); }
    bool confirmPayment(PaymentRequest *request) override { return realGateway->confirmPayment(request); }
};
}

#endif

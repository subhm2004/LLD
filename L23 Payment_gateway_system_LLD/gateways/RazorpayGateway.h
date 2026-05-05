#ifndef PAYMENT_GATEWAY_LLD_GATEWAYS_RAZORPAYGATEWAY_H
#define PAYMENT_GATEWAY_LLD_GATEWAYS_RAZORPAYGATEWAY_H

#include <bits/stdc++.h>

#include "PaymentGateway.h"

using namespace std;

namespace payment_gateway_lld {
class RazorpayGateway : public PaymentGateway {
public:
    RazorpayGateway() { bankingSystem = new RazorpayBankingSystem(); }

    bool validatePayment(PaymentRequest *request) override {
        cout << "[Razorpay] Validating payment for " << request->sender << ".\n";
        return request->amount > 0;
    }
    bool initiatePayment(PaymentRequest *request) override {
        cout << "[Razorpay] Initiating payment of " << request->amount << " " << request->currency
             << " for " << request->sender << ".\n";
        return bankingSystem->processPayment(request->amount);
    }
    bool confirmPayment(PaymentRequest *request) override {
        cout << "[Razorpay] Confirming payment for " << request->sender << ".\n";
        return true;
    }
};
}

#endif

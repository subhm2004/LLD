#ifndef PAYMENT_GATEWAY_LLD_GATEWAYS_PAYTMGATEWAY_H
#define PAYMENT_GATEWAY_LLD_GATEWAYS_PAYTMGATEWAY_H

#include <bits/stdc++.h>

#include "PaymentGateway.h"

using namespace std;

namespace payment_gateway_lld {
class PaytmGateway : public PaymentGateway {
public:
    PaytmGateway() { bankingSystem = new PaytmBankingSystem(); }

    bool validatePayment(PaymentRequest *request) override {
        cout << "[Paytm] Validating payment for " << request->sender << ".\n";
        return request->amount > 0 && request->currency == "INR";
    }
    bool initiatePayment(PaymentRequest *request) override {
        cout << "[Paytm] Initiating payment of " << request->amount << " " << request->currency
             << " for " << request->sender << ".\n";
        return bankingSystem->processPayment(request->amount);
    }
    bool confirmPayment(PaymentRequest *request) override {
        cout << "[Paytm] Confirming payment for " << request->sender << ".\n";
        return true;
    }
};
}

#endif

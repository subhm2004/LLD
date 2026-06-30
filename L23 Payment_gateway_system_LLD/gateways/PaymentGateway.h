// gateways/PaymentGateway.h — TEMPLATE METHOD base: processPayment() ka fixed
// skeleton (validate -> initiate -> confirm). Concrete gateways (Paytm/Razorpay/
// PayPal) sirf apne provider-specific steps override karte hain, order base ke control me.
#ifndef PAYMENT_GATEWAY_LLD_GATEWAYS_PAYMENTGATEWAY_H
#define PAYMENT_GATEWAY_LLD_GATEWAYS_PAYMENTGATEWAY_H

#include <bits/stdc++.h>

#include "../banking/BankingSystem.h"
#include "../models/PaymentRequest.h"

using namespace std;

namespace payment_gateway_lld {
class PaymentGateway {
protected:
    BankingSystem *bankingSystem;

public:
    PaymentGateway() : bankingSystem(nullptr) {}
    virtual ~PaymentGateway() { delete bankingSystem; }

    virtual bool processPayment(PaymentRequest *request) {
        if (!validatePayment(request)) {
            cout << "[PaymentGateway] Validation failed for " << request->sender << ".\n";
            return false;
        }
        if (!initiatePayment(request)) {
            cout << "[PaymentGateway] Initiation failed for " << request->sender << ".\n";
            return false;
        }
        if (!confirmPayment(request)) {
            cout << "[PaymentGateway] Confirmation failed for " << request->sender << ".\n";
            return false;
        }
        return true;
    }

    virtual bool validatePayment(PaymentRequest *request) = 0;
    virtual bool initiatePayment(PaymentRequest *request) = 0;
    virtual bool confirmPayment(PaymentRequest *request) = 0;
};
}

#endif

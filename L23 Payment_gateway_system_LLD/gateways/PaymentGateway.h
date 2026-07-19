// ============================================================================
//  gateways/PaymentGateway.h — TEMPLATE METHOD base (L20): payment ka skeleton
// ----------------------------------------------------------------------------
//  Har payment provider ka flow SAME 3 steps ka hai — validate, initiate,
//  confirm — ISI ORDER me. Par HAR provider ke ye steps alag hote hain
//  (Paytm UPI check, Razorpay account, PayPal currency...).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  TEMPLATE METHOD: processPayment() = FIXED SKELETON                     │
//  │                                                                          │
//  │   bool processPayment(req) {          // <- ye template method hai      │
//  │       if (!validatePayment(req)) return false;   // STEP 1 (abstract)   │
//  │       if (!initiatePayment(req)) return false;   // STEP 2 (abstract)   │
//  │       if (!confirmPayment(req))  return false;   // STEP 3 (abstract)   │
//  │       return true;                                                      │
//  │   }                                                                     │
//  │                                                                          │
//  │  Order base ke CONTROL me (koi provider steps ka order nahi badal       │
//  │  sakta), par HAR step subclass fill karti hai (pure virtual). Kisi bhi  │
//  │  step ne false diya to poora flow fail — fail-fast!                     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  DHYAN DO — har gateway ke paas ek BANKING SYSTEM (Strategy) bhi hai jo
//  actual money process karta hai. Do patterns ek class me: Template Method
//  (flow skeleton) + Strategy (banking backend, composition).
//  (Paytm/Razorpay/PayPal isko extend karte hain — teeno neeche.)
// ============================================================================
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

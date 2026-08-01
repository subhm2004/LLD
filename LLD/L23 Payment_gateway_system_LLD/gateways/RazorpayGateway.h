// ============================================================================
//  gateways/RazorpayGateway.h — Concrete Gateway #2: Razorpay flow
// ----------------------------------------------------------------------------
//  Template Method ki 3 steps, Razorpay style:
//    validate -> amount > 0 (Razorpay any currency accept karta, Paytm nahi!)
//    initiate -> RazorpayBankingSystem (90% success — sabse reliable)
//    confirm  -> receipt (bank A/C numbers, payment ID "pay_...", timestamp)
//  Har gateway ka validate DIFFERENT hai — yahi Template Method ka point:
//    same skeleton (validate->initiate->confirm), alag implementations.
// ============================================================================
#ifndef PAYMENT_GATEWAY_LLD_GATEWAYS_RAZORPAYGATEWAY_H
#define PAYMENT_GATEWAY_LLD_GATEWAYS_RAZORPAYGATEWAY_H

#include <bits/stdc++.h>

#include "../utils/RazorpayTransactionUtil.h"
#include "PaymentGateway.h"

using namespace std;

namespace payment_gateway_lld {
class RazorpayGateway : public PaymentGateway {
    void printTransactionReceipt(PaymentRequest *request) const {
        string fromAccount = RazorpayTransactionUtil::toRazorpayAccount(request->sender);
        string toAccount = RazorpayTransactionUtil::toRazorpayAccount(request->reciever);
        string paymentId = RazorpayTransactionUtil::generatePaymentId();
        string completedAt = RazorpayTransactionUtil::formatCompletedAt();

        cout << "[Razorpay] Payment receipt\n";
        cout << "[Razorpay]   Payer   : " << request->sender << " (A/C " << fromAccount << ")\n";
        cout << "[Razorpay]   Payee   : " << request->reciever << " (A/C " << toAccount << ")\n";
        cout << "[Razorpay]   Transfer: A/C " << fromAccount << " -> A/C " << toAccount << "\n";
        cout << "[Razorpay]   Amount  : " << request->amount << " " << request->currency << "\n";
        cout << "[Razorpay]   Completed at: " << completedAt << "\n";
        cout << "[Razorpay]   Payment ID: " << paymentId << "\n";
    }

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
        printTransactionReceipt(request);
        return true;
    }
};
}

#endif

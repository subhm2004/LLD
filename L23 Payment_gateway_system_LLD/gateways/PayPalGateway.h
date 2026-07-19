// ============================================================================
//  gateways/PayPalGateway.h — Concrete Gateway #3: PayPal flow (international)
// ----------------------------------------------------------------------------
//  Template Method ki 3 steps, PayPal style:
//    validate -> amount > 0 AND currency USD/EUR/GBP (international only —
//                INR reject! isliye demo me PayPal ke liye USD use hota)
//    initiate -> PayPalBankingSystem (85% success)
//    confirm  -> receipt (wallet emails "name@paypal.com", txn ID, timestamp)
//  Teeno gateways compare karo: validate ka rule har ek me alag (INR-only,
//  any, international-only) — same interface, provider-specific logic.
// ============================================================================
#ifndef PAYMENT_GATEWAY_LLD_GATEWAYS_PAYPALGATEWAY_H
#define PAYMENT_GATEWAY_LLD_GATEWAYS_PAYPALGATEWAY_H

#include <bits/stdc++.h>

#include "../utils/PayPalTransactionUtil.h"
#include "PaymentGateway.h"

using namespace std;

namespace payment_gateway_lld {
class PayPalGateway : public PaymentGateway {
    static bool isSupportedCurrency(const string &currency) {
        return currency == "USD" || currency == "EUR" || currency == "GBP";
    }

    void printTransactionReceipt(PaymentRequest *request) const {
        string fromEmail = PayPalTransactionUtil::toPayPalEmail(request->sender);
        string toEmail = PayPalTransactionUtil::toPayPalEmail(request->reciever);
        string transactionId = PayPalTransactionUtil::generateTransactionId();
        string timestamp = PayPalTransactionUtil::formatTimestamp();

        cout << "[PayPal] Payment receipt\n";
        cout << "[PayPal]   Payer   : " << request->sender << " (" << fromEmail << ")\n";
        cout << "[PayPal]   Payee   : " << request->reciever << " (" << toEmail << ")\n";
        cout << "[PayPal]   Transfer: " << fromEmail << " -> " << toEmail << "\n";
        cout << "[PayPal]   Amount  : " << request->amount << " " << request->currency << "\n";
        cout << "[PayPal]   Time    : " << timestamp << "\n";
        cout << "[PayPal]   Txn ID  : " << transactionId << "\n";
    }

public:
    PayPalGateway() { bankingSystem = new PayPalBankingSystem(); }

    bool validatePayment(PaymentRequest *request) override {
        cout << "[PayPal] Validating payment for " << request->sender << ".\n";
        return request->amount > 0 && isSupportedCurrency(request->currency);
    }
    bool initiatePayment(PaymentRequest *request) override {
        cout << "[PayPal] Initiating payment of " << request->amount << " " << request->currency
             << " for " << request->sender << ".\n";
        return bankingSystem->processPayment(request->amount);
    }
    bool confirmPayment(PaymentRequest *request) override {
        cout << "[PayPal] Confirming payment for " << request->sender << ".\n";
        printTransactionReceipt(request);
        return true;
    }
};
}

#endif

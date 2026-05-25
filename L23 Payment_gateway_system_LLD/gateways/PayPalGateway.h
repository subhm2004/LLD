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

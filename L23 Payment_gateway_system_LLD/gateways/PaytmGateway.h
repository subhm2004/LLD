// gateways/PaytmGateway.h — Concrete gateway: Paytm UPI flow. Validate/initiate/
// confirm steps override karta hai (source/dest UPI IDs, timestamp, reference no.).
#ifndef PAYMENT_GATEWAY_LLD_GATEWAYS_PAYTMGATEWAY_H
#define PAYMENT_GATEWAY_LLD_GATEWAYS_PAYTMGATEWAY_H

#include <bits/stdc++.h>

#include "../utils/PaytmTransactionUtil.h"
#include "PaymentGateway.h"

using namespace std;

namespace payment_gateway_lld {
class PaytmGateway : public PaymentGateway {
    void printTransactionReceipt(PaymentRequest *request) const {
        string fromUpi = PaytmTransactionUtil::toPaytmUpi(request->sender);
        string toUpi = PaytmTransactionUtil::toPaytmUpi(request->reciever);
        string reference = PaytmTransactionUtil::generateReferenceNumber();
        string timestamp = PaytmTransactionUtil::formatTimestamp();

        cout << "[Paytm] Payment receipt\n";
        cout << "[Paytm]   Payer   : " << request->sender << " (" << fromUpi << ")\n";
        cout << "[Paytm]   Payee   : " << request->reciever << " (" << toUpi << ")\n";
        cout << "[Paytm]   UPI flow: " << fromUpi << " -> " << toUpi << "\n";
        cout << "[Paytm]   Amount  : " << request->amount << " " << request->currency << "\n";
        cout << "[Paytm]   Time    : " << timestamp << "\n";
        cout << "[Paytm]   Ref no. : " << reference << "\n";
    }

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
        printTransactionReceipt(request);
        return true;
    }
};
}

#endif

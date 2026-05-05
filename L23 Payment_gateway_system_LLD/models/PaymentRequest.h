#ifndef PAYMENT_GATEWAY_LLD_MODELS_PAYMENTREQUEST_H
#define PAYMENT_GATEWAY_LLD_MODELS_PAYMENTREQUEST_H

#include <bits/stdc++.h>
using namespace std;

namespace payment_gateway_lld {
struct PaymentRequest {
    string sender;
    string reciever;
    double amount;
    string currency;

    PaymentRequest(const string &sender, const string &reciever, double amount, const string &currency)
        : sender(sender), reciever(reciever), amount(amount), currency(currency) {}
};
}

#endif

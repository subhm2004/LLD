// ============================================================================
//  models/PaymentRequest.h — Payment request data (sender/receiver/amount/currency)
// ----------------------------------------------------------------------------
//  Ek UNIFIED request model jispe SAARE gateways kaam karte hain — Paytm ho
//  ya PayPal, sab yahi PaymentRequest lete hain. Isi uniform model ki wajah
//  se gateways interchangeable hain. Simple struct (public fields).
//  (NOTE: field ka naam "reciever" hai — spelling typo, par code me
//  consistent hai isliye chalta hai.)
// ============================================================================
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

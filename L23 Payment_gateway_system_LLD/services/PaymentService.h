// ============================================================================
//  services/PaymentService.h — SINGLETON: gateway ko hold karke payment chalata
// ----------------------------------------------------------------------------
//  Controller aur gateway ke beech ka thin orchestration layer. setGateway()
//  se current gateway (proxy-wrapped) set hota hai, processPayment() usse
//  chala deta hai.
//
//  ⭐ OWNERSHIP DETAIL: setGateway() naya gateway set karne se PEHLE purana
//  delete karta hai (leak nahi). Aur ~PaymentService bhi last gateway delete
//  karta. Gateway ke andar Proxy hai, Proxy ke andar real gateway + retry
//  strategy — ye poora chain ek delete se saaf ho jaata (nested destructors).
//  SINGLETON: ek hi payment service (static instance + inline, C++17).
// ============================================================================
#ifndef PAYMENT_GATEWAY_LLD_SERVICES_PAYMENTSERVICE_H
#define PAYMENT_GATEWAY_LLD_SERVICES_PAYMENTSERVICE_H

#include <bits/stdc++.h>

#include "../gateways/PaymentGateway.h"

using namespace std;

namespace payment_gateway_lld {
class PaymentService {
private:
    static PaymentService instance;
    PaymentGateway *gateway;

    PaymentService() : gateway(nullptr) {}
    PaymentService(const PaymentService &) = delete;
    PaymentService &operator=(const PaymentService &) = delete;

public:
    ~PaymentService() { delete gateway; }
    static PaymentService &getInstance() { return instance; }

    void setGateway(PaymentGateway *g) {
        if (gateway) delete gateway;
        gateway = g;
    }

    bool processPayment(PaymentRequest *request) {
        if (!gateway) {
            cout << "[PaymentService] No payment gateway selected.\n";
            return false;
        }
        return gateway->processPayment(request);
    }
};

inline PaymentService PaymentService::instance;
}

#endif

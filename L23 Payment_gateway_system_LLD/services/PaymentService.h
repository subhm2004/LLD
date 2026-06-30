// services/PaymentService.h — SINGLETON service: factory se sahi gateway leke
// payment process karwata hai. Controller aur gateways ke beech orchestration layer.
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

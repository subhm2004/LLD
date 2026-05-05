#ifndef PAYMENT_GATEWAY_LLD_CONTROLLERS_PAYMENTCONTROLLER_H
#define PAYMENT_GATEWAY_LLD_CONTROLLERS_PAYMENTCONTROLLER_H

#include <bits/stdc++.h>

#include "../factories/GatewayFactory.h"
#include "../services/PaymentService.h"

using namespace std;

namespace payment_gateway_lld {
class PaymentController {
private:
    static PaymentController instance;
    PaymentController() {}
    PaymentController(const PaymentController &) = delete;
    PaymentController &operator=(const PaymentController &) = delete;

public:
    static PaymentController &getInstance() { return instance; }

    bool handlePayment(GatewayType type, PaymentRequest *request) {
        PaymentGateway *gateway = GatewayFactory::getInstance().getGateway(type);
        PaymentService::getInstance().setGateway(gateway);
        return PaymentService::getInstance().processPayment(request);
    }
};
}

#endif

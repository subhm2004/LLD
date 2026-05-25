#ifndef PAYMENT_GATEWAY_LLD_CONTROLLERS_PAYMENTCONTROLLER_H
#define PAYMENT_GATEWAY_LLD_CONTROLLERS_PAYMENTCONTROLLER_H

#include <bits/stdc++.h>

#include "../enums/BillingInterval.h"
#include "../enums/GatewayType.h"
#include "../enums/RetryStrategyType.h"
#include "../factories/GatewayFactory.h"
#include "../services/PaymentService.h"
#include "../services/RecurringPaymentService.h"

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

    bool handlePayment(GatewayType type, PaymentRequest *request,
                       RetryStrategyType retryType = RetryStrategyType::LINEAR) {
        PaymentGateway *gateway = GatewayFactory::getInstance().getGateway(type, retryType);
        PaymentService::getInstance().setGateway(gateway);
        return PaymentService::getInstance().processPayment(request);
    }

    string createSubscription(const string &subscriber, const string &receiver, double amount,
                              const string &currency, GatewayType gateway, BillingInterval interval) {
        return RecurringPaymentService::getInstance().registerSubscription(
            subscriber, receiver, amount, currency, gateway, interval);
    }

    bool cancelSubscription(const string &subscriptionId) {
        return RecurringPaymentService::getInstance().cancelSubscription(subscriptionId);
    }

    int processRecurringBilling(RetryStrategyType retryType = RetryStrategyType::LINEAR) {
        return RecurringPaymentService::getInstance().processBillingCycle(retryType);
    }
};

inline PaymentController PaymentController::instance;
}

#endif

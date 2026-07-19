// ============================================================================
//  controllers/PaymentController.h — SINGLE entry-point (SINGLETON + Facade-ish)
// ----------------------------------------------------------------------------
//  Client (main.cpp) ke liye ekmatra darwaza. 4 simple methods:
//    handlePayment()          -> one-time payment (factory se gateway leke)
//    createSubscription()     -> recurring payment register
//    cancelSubscription()     -> subscription band
//    processRecurringBilling()-> saari active subscriptions ko charge karo
//
//  Client ko GatewayFactory, Proxy, PaymentService, RetryStrategy... in sab
//  ke jhamele ka kuch pata nahi — controller sab delegate karta hai. Facade
//  jaisa role, par ye "controller" hai (request handle karta hai).
//
//  SINGLETON style yahan MODERN hai: `static PaymentController instance;` +
//  `inline` definition (C++17) + deleted copy/assignment. Ek hi instance,
//  clean. (L10 me singleton styles ka comparison hai — ye eager static
//  member wala hai, thread-safe by static-init.)
// ============================================================================
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

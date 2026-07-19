// ============================================================================
//  services/RecurringPaymentService.h — SINGLETON: subscriptions manager
// ----------------------------------------------------------------------------
//  Netflix/Spotify jaise recurring payments handle karta hai:
//    registerSubscription() -> nayi subscription banao (SUB-1, SUB-2...)
//    cancelSubscription()   -> active flag false (soft delete)
//    processBillingCycle()  -> saari ACTIVE subscriptions ko charge karo
//
//  ⭐ REUSE: billing cycle me har active subscription ke liye WAHI payment
//  flow use hota hai (GatewayFactory -> proxy -> gateway -> retry) jo
//  one-time payment me tha! Ek hi payment infrastructure, do use-cases
//  (one-time + recurring). Yahi achha layered design hai.
//
//  Cancel "soft" hai (active=false, object delete nahi) — history/audit ke
//  liye subscription record rehta hai. billing cycle inactive ko skip karta.
//  SINGLETON: ek hi subscription registry (static instance + inline).
// ============================================================================
#ifndef PAYMENT_GATEWAY_LLD_SERVICES_RECURRINGPAYMENTSERVICE_H
#define PAYMENT_GATEWAY_LLD_SERVICES_RECURRINGPAYMENTSERVICE_H

#include <bits/stdc++.h>

#include "../enums/BillingInterval.h"
#include "../enums/GatewayType.h"
#include "../enums/RetryStrategyType.h"
#include "../factories/GatewayFactory.h"
#include "../models/PaymentRequest.h"
#include "../models/Subscription.h"
#include "../services/PaymentService.h"

using namespace std;

namespace payment_gateway_lld {
class RecurringPaymentService {
private:
    static RecurringPaymentService instance;
    vector<Subscription *> subscriptions;
    int nextSubscriptionId;

    RecurringPaymentService() : nextSubscriptionId(1) {}
    RecurringPaymentService(const RecurringPaymentService &) = delete;
    RecurringPaymentService &operator=(const RecurringPaymentService &) = delete;

    static string intervalLabel(BillingInterval interval) {
        return interval == BillingInterval::WEEKLY ? "WEEKLY" : "MONTHLY";
    }

public:
    ~RecurringPaymentService() {
        for (Subscription *sub : subscriptions) delete sub;
    }
    static RecurringPaymentService &getInstance() { return instance; }

    string registerSubscription(const string &subscriber, const string &receiver, double amount,
                                const string &currency, GatewayType gateway,
                                BillingInterval interval) {
        string id = "SUB-" + to_string(nextSubscriptionId++);
        Subscription *sub =
            new Subscription(id, subscriber, receiver, amount, currency, gateway, interval);
        subscriptions.push_back(sub);
        cout << "[RecurringPayment] Registered " << id << " (" << intervalLabel(interval)
             << ", " << amount << " " << currency << ") for " << subscriber << ".\n";
        return id;
    }

    bool cancelSubscription(const string &subscriptionId) {
        for (Subscription *sub : subscriptions) {
            if (sub->subscriptionId == subscriptionId && sub->active) {
                sub->active = false;
                cout << "[RecurringPayment] Cancelled " << subscriptionId << ".\n";
                return true;
            }
        }
        cout << "[RecurringPayment] Subscription not found or already inactive: " << subscriptionId
             << ".\n";
        return false;
    }

    int processBillingCycle(RetryStrategyType retryType = RetryStrategyType::LINEAR) {
        cout << "[RecurringPayment] Processing billing cycle...\n";
        int charged = 0;
        for (Subscription *sub : subscriptions) {
            if (!sub->active) continue;

            cout << "[RecurringPayment] Charging " << sub->subscriptionId << " ("
                 << intervalLabel(sub->interval) << ") via gateway.\n";
            PaymentRequest request(sub->subscriber, sub->receiver, sub->amount, sub->currency);
            PaymentGateway *gateway =
                GatewayFactory::getInstance().getGateway(sub->gateway, retryType);
            PaymentService::getInstance().setGateway(gateway);
            if (PaymentService::getInstance().processPayment(&request)) {
                ++charged;
            }
        }
        cout << "[RecurringPayment] Billing cycle complete. Charged " << charged << " subscription(s).\n";
        return charged;
    }
};

inline RecurringPaymentService RecurringPaymentService::instance;
}

#endif

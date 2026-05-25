#ifndef PAYMENT_GATEWAY_LLD_MODELS_SUBSCRIPTION_H
#define PAYMENT_GATEWAY_LLD_MODELS_SUBSCRIPTION_H

#include <bits/stdc++.h>

#include "../enums/BillingInterval.h"
#include "../enums/GatewayType.h"

using namespace std;

namespace payment_gateway_lld {
struct Subscription {
    string subscriptionId;
    string subscriber;
    string receiver;
    double amount;
    string currency;
    GatewayType gateway;
    BillingInterval interval;
    bool active;

    Subscription(const string &subscriptionId, const string &subscriber, const string &receiver,
                 double amount, const string &currency, GatewayType gateway, BillingInterval interval)
        : subscriptionId(subscriptionId), subscriber(subscriber), receiver(receiver), amount(amount),
          currency(currency), gateway(gateway), interval(interval), active(true) {}
};
}

#endif

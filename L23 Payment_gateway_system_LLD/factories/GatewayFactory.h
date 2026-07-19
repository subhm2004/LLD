// ============================================================================
//  factories/GatewayFactory.h — FACTORY (Singleton): gateway + proxy assembler
// ----------------------------------------------------------------------------
//  Ye is project ka SABSE IMPORTANT factory hai — 3 patterns ko ek saath
//  assemble karta hai! getGateway() ke andar:
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │   1. RetryStrategyFactory se retry strategy banao   [FACTORY]           │
//  │   2. GatewayType dekh ke sahi real gateway banao     [FACTORY]          │
//  │      (PaytmGateway / RazorpayGateway / PayPalGateway) [TEMPLATE METHOD] │
//  │   3. real gateway ko PROXY me wrap karo (retry ke saath) [PROXY]        │
//  │                                                                          │
//  │   return new PaymentGatewayProxy(new PaytmGateway(), strategy);         │
//  │              ^^^^^ PROXY          ^^^^^ real gateway  ^^^^ retry strategy│
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  Client ko HAMESHA ek ready-to-use PROXIED gateway milta hai — usse pata
//  hi nahi ki andar proxy + real gateway + retry strategy ka poora setup hai.
//  Yahi factory ka fayda: complex object assembly EK jagah chhupi hui.
//
//  defaultRetryConfig() har gateway ke liye alag retry config deta (Paytm 3x
//  200ms, PayPal 3x 150ms...) — provider-specific tuning. SINGLETON: ek factory.
// ============================================================================
#ifndef PAYMENT_GATEWAY_LLD_FACTORIES_GATEWAYFACTORY_H
#define PAYMENT_GATEWAY_LLD_FACTORIES_GATEWAYFACTORY_H

#include <bits/stdc++.h>

#include "../enums/GatewayType.h"
#include "../enums/RetryStrategyType.h"
#include "../gateways/PayPalGateway.h"
#include "../gateways/PaytmGateway.h"
#include "../gateways/RazorpayGateway.h"
#include "../proxy/PaymentGatewayProxy.h"
#include "../retry/RetryStrategyFactory.h"

using namespace std;

namespace payment_gateway_lld {
class GatewayFactory {
private:
    static GatewayFactory instance;
    GatewayFactory() {}
    GatewayFactory(const GatewayFactory &) = delete;
    GatewayFactory &operator=(const GatewayFactory &) = delete;

    static pair<int, int> defaultRetryConfig(GatewayType type) {
        if (type == GatewayType::PAYTM) return {3, 200};
        if (type == GatewayType::PAYPAL) return {3, 150};
        return {3, 100};
    }

public:
    static GatewayFactory &getInstance() { return instance; }

    PaymentGateway *getGateway(GatewayType type,
                               RetryStrategyType retryType = RetryStrategyType::LINEAR) {
        auto config = defaultRetryConfig(type);
        RetryStrategy *strategy =
            RetryStrategyFactory::create(retryType, config.first, config.second);
        if (type == GatewayType::PAYTM) {
            return new PaymentGatewayProxy(new PaytmGateway(), strategy);
        }
        if (type == GatewayType::PAYPAL) {
            return new PaymentGatewayProxy(new PayPalGateway(), strategy);
        }
        return new PaymentGatewayProxy(new RazorpayGateway(), strategy);
    }
};

inline GatewayFactory GatewayFactory::instance;
}

#endif

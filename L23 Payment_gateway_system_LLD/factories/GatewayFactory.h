// factories/GatewayFactory.h — FACTORY (Singleton): GatewayType ke hisaab se
// sahi gateway banata hai, AUR usse retry-proxy me wrap karke deta hai. Client
// ko hamesha ek ready-to-use proxied gateway milta hai.
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

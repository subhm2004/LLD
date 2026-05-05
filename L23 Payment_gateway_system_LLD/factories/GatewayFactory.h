#ifndef PAYMENT_GATEWAY_LLD_FACTORIES_GATEWAYFACTORY_H
#define PAYMENT_GATEWAY_LLD_FACTORIES_GATEWAYFACTORY_H

#include <bits/stdc++.h>

#include "../enums/GatewayType.h"
#include "../gateways/PaytmGateway.h"
#include "../gateways/RazorpayGateway.h"
#include "../proxy/PaymentGatewayProxy.h"

using namespace std;

namespace payment_gateway_lld {
class GatewayFactory {
private:
    static GatewayFactory instance;
    GatewayFactory() {}
    GatewayFactory(const GatewayFactory &) = delete;
    GatewayFactory &operator=(const GatewayFactory &) = delete;

public:
    static GatewayFactory &getInstance() { return instance; }

    PaymentGateway *getGateway(GatewayType type) {
        if (type == GatewayType::PAYTM) {
            return new PaymentGatewayProxy(new PaytmGateway(), 3);
        }
        return new PaymentGatewayProxy(new RazorpayGateway(), 1);
    }
};
}

#endif

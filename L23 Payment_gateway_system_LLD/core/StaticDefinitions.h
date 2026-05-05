#ifndef PAYMENT_GATEWAY_LLD_CORE_STATICDEFINITIONS_H
#define PAYMENT_GATEWAY_LLD_CORE_STATICDEFINITIONS_H

#include <bits/stdc++.h>

#include "../controllers/PaymentController.h"
#include "../factories/GatewayFactory.h"
#include "../services/PaymentService.h"

using namespace std;

namespace payment_gateway_lld {
GatewayFactory GatewayFactory::instance;
PaymentService PaymentService::instance;
PaymentController PaymentController::instance;
}

#endif

#include "../controllers/PaymentController.h"
#include "../factories/GatewayFactory.h"
#include "../services/PaymentService.h"

namespace payment_gateway_lld {
GatewayFactory GatewayFactory::instance;
PaymentService PaymentService::instance;
PaymentController PaymentController::instance;
}

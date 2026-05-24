#ifndef RAZORPAY_LLD_CORE_RAZORPAY_PAYMENT_SYSTEM_H
#define RAZORPAY_LLD_CORE_RAZORPAY_PAYMENT_SYSTEM_H

#include <string>

#include "../clients/RazorpayApiClient.h"
#include "../enums/PaymentMethod.h"
#include "../flows/CheckoutFlowTemplate.h"
#include "../models/CheckoutInput.h"
#include "../models/MerchantConfig.h"
#include "../models/Refund.h"
#include "../models/RazorpayOrder.h"
#include "../models/RazorpayPayment.h"
#include "../services/CaptureService.h"
#include "../services/IdempotencyService.h"
#include "../services/OrderService.h"
#include "../services/PaymentService.h"
#include "../services/RefundService.h"
#include "../services/WebhookService.h"

namespace razorpay_lld {

// Facade — external clients interact only through this class
class RazorpayPaymentSystem {
public:
    explicit RazorpayPaymentSystem(MerchantConfig config)
        : config_(std::move(config)),
          client_(config_),
          orderService_(&client_, &idempotency_),
          paymentService_(&client_, &orderService_, &idempotency_),
          captureService_(&client_, &paymentService_, &orderService_),
          refundService_(&client_, &paymentService_),
          webhookService_(config_, &paymentService_, &captureService_, &refundService_),
          checkoutFlow_(&orderService_, &paymentService_, &captureService_) {}

    RazorpayOrder createOrder(int amountPaise, const std::string& currency,
                              const std::string& receipt, const std::string& idempotencyKey = "") {
        return orderService_.createOrder(amountPaise, currency, receipt, idempotencyKey);
    }

    RazorpayPayment initiatePayment(const std::string& orderId, PaymentMethod method,
                                    const std::string& instrumentDetail,
                                    bool simulateSuccess = true,
                                    const std::string& idempotencyKey = "") {
        return paymentService_.initiatePayment(orderId, method, instrumentDetail, simulateSuccess,
                                               idempotencyKey);
    }

    RazorpayPayment capturePayment(const std::string& paymentId, int amountPaise) {
        return captureService_.capture(paymentId, amountPaise);
    }

    RazorpayPayment checkout(const CheckoutInput& input) { return checkoutFlow_.runCheckout(input); }

    void processWebhook(const std::string& payload, const std::string& signature) {
        webhookService_.handle(payload, signature);
    }

    Refund refundPayment(const std::string& paymentId, int amountPaise) {
        return refundService_.createRefund(paymentId, amountPaise);
    }

    RazorpayOrder getOrder(const std::string& orderId) const {
        return orderService_.getOrder(orderId);
    }

    RazorpayPayment getPayment(const std::string& paymentId) const {
        return paymentService_.getPayment(paymentId);
    }

    Refund getRefund(const std::string& refundId) const { return refundService_.getRefund(refundId); }

private:
    MerchantConfig config_;
    RazorpayApiClient client_;
    IdempotencyService idempotency_;
    OrderService orderService_;
    PaymentService paymentService_;
    CaptureService captureService_;
    RefundService refundService_;
    WebhookService webhookService_;
    CheckoutFlowTemplate checkoutFlow_;
};

}  // namespace razorpay_lld

#endif

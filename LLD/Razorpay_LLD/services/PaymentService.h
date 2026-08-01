#ifndef RAZORPAY_LLD_SERVICES_PAYMENT_SERVICE_H
#define RAZORPAY_LLD_SERVICES_PAYMENT_SERVICE_H

#include <stdexcept>
#include <unordered_map>

#include "../clients/RazorpayApiClient.h"
#include "../enums/OrderStatus.h"
#include "../enums/PaymentStatus.h"
#include "../factories/EntityIdFactory.h"
#include "../factories/PaymentMethodValidatorFactory.h"
#include "../models/RazorpayPayment.h"
#include "IdempotencyService.h"
#include "OrderService.h"

namespace razorpay_lld {

// Initiates payments only — capture delegated to CaptureService
class PaymentService {
public:
    PaymentService(RazorpayApiClient* client, OrderService* orders, IdempotencyService* idempotency)
        : client_(client), orders_(orders), idempotency_(idempotency) {}

    RazorpayPayment initiatePayment(const std::string& orderId, PaymentMethod method,
                                    const std::string& instrumentDetail,
                                    bool simulateSuccess = true,
                                    const std::string& idempotencyKey = "") {
        if (!idempotencyKey.empty() && idempotency_->exists(idempotencyKey)) {
            return payments_.at(idempotency_->resolveDuplicate(idempotencyKey));
        }

        const RazorpayOrder& order = orders_->getOrder(orderId);
        if (order.status == OrderStatus::PAID) {
            throw std::runtime_error("order already paid: " + orderId);
        }

        auto validator = PaymentMethodValidatorFactory::create(method);
        validator->validate(instrumentDetail);

        RazorpayPayment payment;
        payment.paymentId = EntityIdFactory::nextPaymentId();
        payment.orderId = orderId;
        payment.amountPaise = order.amountPaise;
        payment.method = method;
        payment.idempotencyKey = idempotencyKey;

        orders_->markAttempted(orderId);
        client_->postPayment(payment.paymentId, orderId, paymentMethodToString(method));

        if (simulateSuccess) {
            payment.status = PaymentStatus::AUTHORIZED;
        } else {
            payment.status = PaymentStatus::FAILED;
            payment.failureReason = "issuer declined";
        }

        payments_[payment.paymentId] = payment;
        if (!idempotencyKey.empty()) {
            idempotency_->store(idempotencyKey, payment.paymentId);
        }
        return payment;
    }

    RazorpayPayment& getPayment(const std::string& paymentId) {
        auto it = payments_.find(paymentId);
        if (it == payments_.end()) {
            throw std::runtime_error("payment not found: " + paymentId);
        }
        return it->second;
    }

    const RazorpayPayment& getPayment(const std::string& paymentId) const {
        return const_cast<PaymentService*>(this)->getPayment(paymentId);
    }

    void markAuthorized(const std::string& paymentId) {
        RazorpayPayment& payment = getPayment(paymentId);
        if (payment.status == PaymentStatus::FAILED) {
            throw std::runtime_error("cannot authorize failed payment");
        }
        payment.status = PaymentStatus::AUTHORIZED;
        payment.failureReason.clear();
    }

    void markFailed(const std::string& paymentId, const std::string& reason) {
        RazorpayPayment& payment = getPayment(paymentId);
        payment.status = PaymentStatus::FAILED;
        payment.failureReason = reason;
    }

    void applyCapture(const std::string& paymentId, int capturedAmountPaise) {
        RazorpayPayment& payment = getPayment(paymentId);
        payment.capturedAmountPaise = capturedAmountPaise;
        payment.status = PaymentStatus::CAPTURED;
    }

    void applyRefund(const std::string& paymentId, int refundAmountPaise) {
        RazorpayPayment& payment = getPayment(paymentId);
        payment.refundedAmountPaise += refundAmountPaise;
        if (payment.refundedAmountPaise >= payment.capturedAmountPaise) {
            payment.status = PaymentStatus::REFUNDED;
        }
    }

private:
    RazorpayApiClient* client_;
    OrderService* orders_;
    IdempotencyService* idempotency_;
    std::unordered_map<std::string, RazorpayPayment> payments_;
};

}  // namespace razorpay_lld

#endif

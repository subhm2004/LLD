#ifndef RAZORPAY_LLD_SERVICES_CAPTURE_SERVICE_H
#define RAZORPAY_LLD_SERVICES_CAPTURE_SERVICE_H

#include <stdexcept>

#include "../clients/RazorpayApiClient.h"
#include "../enums/PaymentStatus.h"
#include "OrderService.h"
#include "PaymentService.h"

namespace razorpay_lld {

class CaptureService {
public:
    CaptureService(RazorpayApiClient* client, PaymentService* payments, OrderService* orders)
        : client_(client), payments_(payments), orders_(orders) {}

    RazorpayPayment capture(const std::string& paymentId, int amountPaise) {
        RazorpayPayment& payment = payments_->getPayment(paymentId);
        if (payment.status != PaymentStatus::AUTHORIZED) {
            throw std::runtime_error("only authorized payments can be captured");
        }
        if (amountPaise <= 0 || amountPaise > payment.amountPaise) {
            throw std::invalid_argument("invalid capture amount");
        }

        client_->postCapture(paymentId, amountPaise);
        payments_->applyCapture(paymentId, amountPaise);
        orders_->markPaid(payment.orderId);
        return payments_->getPayment(paymentId);
    }

private:
    RazorpayApiClient* client_;
    PaymentService* payments_;
    OrderService* orders_;
};

}  // namespace razorpay_lld

#endif

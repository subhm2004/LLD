#ifndef RAZORPAY_LLD_MODELS_RAZORPAY_PAYMENT_H
#define RAZORPAY_LLD_MODELS_RAZORPAY_PAYMENT_H

#include <string>

#include "../enums/PaymentMethod.h"
#include "../enums/PaymentStatus.h"

namespace razorpay_lld {

struct RazorpayPayment {
    std::string paymentId;
    std::string orderId;
    int amountPaise{0};
    PaymentMethod method{PaymentMethod::UPI};
    PaymentStatus status{PaymentStatus::CREATED};
    int capturedAmountPaise{0};
    int refundedAmountPaise{0};
    std::string failureReason;
    std::string idempotencyKey;
};

}  // namespace razorpay_lld

#endif

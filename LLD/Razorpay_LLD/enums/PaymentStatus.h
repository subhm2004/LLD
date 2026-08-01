#ifndef RAZORPAY_LLD_ENUMS_PAYMENT_STATUS_H
#define RAZORPAY_LLD_ENUMS_PAYMENT_STATUS_H

#include <string>

namespace razorpay_lld {

enum class PaymentStatus { CREATED, AUTHORIZED, CAPTURED, FAILED, REFUNDED };

inline std::string paymentStatusToString(PaymentStatus status) {
    switch (status) {
        case PaymentStatus::CREATED:
            return "created";
        case PaymentStatus::AUTHORIZED:
            return "authorized";
        case PaymentStatus::CAPTURED:
            return "captured";
        case PaymentStatus::FAILED:
            return "failed";
        case PaymentStatus::REFUNDED:
            return "refunded";
        default:
            return "unknown";
    }
}

}  // namespace razorpay_lld

#endif

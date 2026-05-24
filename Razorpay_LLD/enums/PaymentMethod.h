#ifndef RAZORPAY_LLD_ENUMS_PAYMENT_METHOD_H
#define RAZORPAY_LLD_ENUMS_PAYMENT_METHOD_H

#include <string>

namespace razorpay_lld {

enum class PaymentMethod { UPI, CARD, WALLET };

inline std::string paymentMethodToString(PaymentMethod method) {
    switch (method) {
        case PaymentMethod::UPI:
            return "upi";
        case PaymentMethod::CARD:
            return "card";
        case PaymentMethod::WALLET:
            return "wallet";
        default:
            return "unknown";
    }
}

}  // namespace razorpay_lld

#endif

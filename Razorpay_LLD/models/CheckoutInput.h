#ifndef RAZORPAY_LLD_MODELS_CHECKOUT_INPUT_H
#define RAZORPAY_LLD_MODELS_CHECKOUT_INPUT_H

#include <string>

#include "../enums/PaymentMethod.h"

namespace razorpay_lld {

struct CheckoutInput {
    int amountPaise{0};
    std::string currency{"INR"};
    std::string receipt;
    PaymentMethod method{PaymentMethod::UPI};
    std::string instrumentDetail;
};

}  // namespace razorpay_lld

#endif

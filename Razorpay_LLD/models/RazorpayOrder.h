#ifndef RAZORPAY_LLD_MODELS_RAZORPAY_ORDER_H
#define RAZORPAY_LLD_MODELS_RAZORPAY_ORDER_H

#include <string>

#include "../enums/OrderStatus.h"

namespace razorpay_lld {

struct RazorpayOrder {
    std::string orderId;
    int amountPaise{0};
    std::string currency{"INR"};
    std::string receipt;
    OrderStatus status{OrderStatus::CREATED};
    std::string idempotencyKey;
};

}  // namespace razorpay_lld

#endif

#ifndef RAZORPAY_LLD_ENUMS_ORDER_STATUS_H
#define RAZORPAY_LLD_ENUMS_ORDER_STATUS_H

#include <string>

namespace razorpay_lld {

enum class OrderStatus { CREATED, PAID, ATTEMPTED };

inline std::string orderStatusToString(OrderStatus status) {
    switch (status) {
        case OrderStatus::CREATED:
            return "created";
        case OrderStatus::PAID:
            return "paid";
        case OrderStatus::ATTEMPTED:
            return "attempted";
        default:
            return "unknown";
    }
}

}  // namespace razorpay_lld

#endif

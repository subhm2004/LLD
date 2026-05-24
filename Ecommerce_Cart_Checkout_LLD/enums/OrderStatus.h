#ifndef ECOMMERCE_CART_CHECKOUT_LLD_ENUMS_ORDERSTATUS_H
#define ECOMMERCE_CART_CHECKOUT_LLD_ENUMS_ORDERSTATUS_H

#include <bits/stdc++.h>

namespace ecommerce_checkout_lld {

using namespace std;

enum class OrderStatus {
    CREATED,
    PAYMENT_PENDING,
    CONFIRMED,
    SHIPPED,
    DELIVERED,
    CANCELLED,
    FAILED
};

inline string orderStatusToString(OrderStatus status) {
    switch (status) {
        case OrderStatus::CREATED:
            return "CREATED";
        case OrderStatus::PAYMENT_PENDING:
            return "PAYMENT_PENDING";
        case OrderStatus::CONFIRMED:
            return "CONFIRMED";
        case OrderStatus::SHIPPED:
            return "SHIPPED";
        case OrderStatus::DELIVERED:
            return "DELIVERED";
        case OrderStatus::CANCELLED:
            return "CANCELLED";
        case OrderStatus::FAILED:
            return "FAILED";
    }
    return "UNKNOWN";
}

}  // namespace ecommerce_checkout_lld

#endif

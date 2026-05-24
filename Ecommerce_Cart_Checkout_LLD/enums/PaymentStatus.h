#ifndef ECOMMERCE_CART_CHECKOUT_LLD_ENUMS_PAYMENTSTATUS_H
#define ECOMMERCE_CART_CHECKOUT_LLD_ENUMS_PAYMENTSTATUS_H

#include <bits/stdc++.h>

namespace ecommerce_checkout_lld {

using namespace std;

enum class PaymentStatus { PENDING, SUCCESS, FAILED };

inline string paymentStatusToString(PaymentStatus status) {
    switch (status) {
        case PaymentStatus::PENDING:
            return "PENDING";
        case PaymentStatus::SUCCESS:
            return "SUCCESS";
        case PaymentStatus::FAILED:
            return "FAILED";
    }
    return "UNKNOWN";
}

}  // namespace ecommerce_checkout_lld

#endif

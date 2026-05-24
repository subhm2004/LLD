#ifndef ECOMMERCE_CART_CHECKOUT_LLD_ENUMS_PAYMENTMETHOD_H
#define ECOMMERCE_CART_CHECKOUT_LLD_ENUMS_PAYMENTMETHOD_H

#include <bits/stdc++.h>

namespace ecommerce_checkout_lld {

using namespace std;

enum class PaymentMethod { UPI, CREDIT_CARD, COD };

inline string paymentMethodToString(PaymentMethod method) {
    switch (method) {
        case PaymentMethod::UPI:
            return "UPI";
        case PaymentMethod::CREDIT_CARD:
            return "CREDIT_CARD";
        case PaymentMethod::COD:
            return "COD";
    }
    return "UNKNOWN";
}

}  // namespace ecommerce_checkout_lld

#endif

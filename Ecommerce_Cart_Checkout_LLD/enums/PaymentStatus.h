// ============================================================================
//  enums/PaymentStatus.h  —  Paise ki haalat (order ki haalat se ALAG)
// ----------------------------------------------------------------------------
//     PENDING -> paisa abhi aana baaki (COD me delivery pe milega)
//     SUCCESS -> paisa mil gaya
//     FAILED  -> payment fail (card decline, invalid amount, etc.)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ ORDER status vs PAYMENT status — do alag cheezein kyun?               │
//  │                                                                          │
//  │  Kyunki dono HAMESHA saath nahi chalte. Sabse achha example COD hai:      │
//  │                                                                          │
//  │     COD order  ->  OrderStatus  = CONFIRMED  (order pakka hai, bhej denge)│
//  │                    PaymentStatus = PENDING   (paisa delivery pe milega)   │
//  │                                                                          │
//  │  Agar ek hi enum hota to ye haalat express hi na hoti — "confirmed but    │
//  │  unpaid" ka koi single value nahi banta. Do alag dimensions = do enums.  │
//  │  (UPI/Card me dono saath chalte: CONFIRMED + SUCCESS.)                   │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_ENUMS_PAYMENTSTATUS_H
#define ECOMMERCE_CART_CHECKOUT_LLD_ENUMS_PAYMENTSTATUS_H

#include <string>

namespace ecommerce_checkout_lld {

using namespace std;

enum class PaymentStatus { PENDING, SUCCESS, FAILED };

inline string paymentStatusToString(PaymentStatus status) {
    switch (status) {
        case PaymentStatus::PENDING: return "PENDING";
        case PaymentStatus::SUCCESS: return "SUCCESS";
        case PaymentStatus::FAILED:  return "FAILED";
    }
    return "UNKNOWN";
}

}  // namespace ecommerce_checkout_lld

#endif

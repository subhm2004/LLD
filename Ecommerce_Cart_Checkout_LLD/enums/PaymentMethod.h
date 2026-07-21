// ============================================================================
//  enums/PaymentMethod.h  —  User kis tareeke se paisa dega
// ----------------------------------------------------------------------------
//     UPI         -> UPI id se (turant paisa)
//     CREDIT_CARD -> card se (turant paisa, bank decline kar sakta)
//     COD         -> Cash On Delivery (paisa BAAD me, delivery pe)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Ye enum FACTORY ka input hai (Factory Pattern)                        │
//  │                                                                          │
//  │  PaymentStrategyFactory::create(method) is enum ko dekh ke sahi           │
//  │  IPaymentStrategy object bana ke deta hai:                                │
//  │      UPI -> UpiPaymentStrategy, CREDIT_CARD -> CardPaymentStrategy, ...   │
//  │                                                                          │
//  │  Isse baaki system ko `if (method == UPI) ... else if ...` likhna hi      │
//  │  nahi padta — wo bas strategy maangta hai aur `pay()` bula deta hai.      │
//  │  (Enum -> object banane ka faisla EK jagah = factory.)                   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ COD KHAAS hai: usme paisa TURANT nahi milta. Isi liye CheckoutService COD
//     ke liye order CONFIRMED karta hai par PaymentStatus PENDING rakhta hai
//     (delivery pe collect hoga). Baaki methods me dono SUCCESS/CONFIRMED.
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_ENUMS_PAYMENTMETHOD_H
#define ECOMMERCE_CART_CHECKOUT_LLD_ENUMS_PAYMENTMETHOD_H

#include <string>

namespace ecommerce_checkout_lld {

using namespace std;

enum class PaymentMethod { UPI, CREDIT_CARD, COD };

inline string paymentMethodToString(PaymentMethod method) {
    switch (method) {
        case PaymentMethod::UPI:         return "UPI";
        case PaymentMethod::CREDIT_CARD: return "CREDIT_CARD";
        case PaymentMethod::COD:         return "COD";
    }
    return "UNKNOWN";
}

}  // namespace ecommerce_checkout_lld

#endif

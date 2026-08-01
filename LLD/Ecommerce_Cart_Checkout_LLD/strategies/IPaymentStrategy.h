// ============================================================================
//  strategies/IPaymentStrategy.h  —  STRATEGY interface #2: paisa kaise lein
// ----------------------------------------------------------------------------
//  "Payment kaise process ho?" — UPI, card, COD, wallet, net-banking... har
//  method ka apna flow aur apne validations. Isliye interface ke peeche.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `paymentInstrument` ek GENERIC string kyun hai?                       │
//  │                                                                          │
//  │  Har method ko alag cheez chahiye:                                        │
//  │     UPI  -> "shubham@okbank"     (UPI id)                                 │
//  │     Card -> "4111111111114242"   (card number)                            │
//  │     COD  -> kuch nahi            (ignore karta hai)                       │
//  │                                                                          │
//  │  Ek hi generic string se interface SAB methods ke liye kaam kar jaata     │
//  │  hai — har method apne hisaab se use interpret/validate karta hai.       │
//  │  ⚠ Trade-off: type-safety kam (compiler check nahi karta ki sahi format   │
//  │  hai). Zyada methods/fields aayein to alag `PaymentDetails` struct behtar │
//  │  hota. Interview me ye trade-off bolna accha point hai.                  │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Return `PaymentResult` hai (bool nahi, exception nahi) — kyunki payment
//     fail hona NORMAL hai (card decline). Result me status + txnId + reason
//     sab aata hai, to caller smart decision le sakta (reason user ko dikhao).
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_STRATEGIES_IPAYMENTSTRATEGY_H
#define ECOMMERCE_CART_CHECKOUT_LLD_STRATEGIES_IPAYMENTSTRATEGY_H

#include <string>

#include "../models/PaymentResult.h"

namespace ecommerce_checkout_lld {

class IPaymentStrategy {
public:
    virtual ~IPaymentStrategy() = default;

    // Amount + instrument (UPI id / card no / ignored) -> payment ka natija.
    virtual PaymentResult pay(double amount, const std::string &paymentInstrument) const = 0;
};

}  // namespace ecommerce_checkout_lld

#endif

// ============================================================================
//  strategies/CodPaymentStrategy.h  —  CONCRETE: Cash On Delivery
// ----------------------------------------------------------------------------
//  COD me abhi paisa nahi milta — bas order ACCEPT hota hai, paisa delivery pe.
//  Isliye ye sabse simple strategy hai: amount valid ho to "accepted".
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `paymentInstrument` ka naam COMMENT me kyun hai?                      │
//  │                                                                          │
//  │     PaymentResult pay(double amount, const string &/*paymentInstrument*/) │
//  │                                                                          │
//  │  COD ko koi UPI-id/card chahiye hi nahi. Par interface (IPaymentStrategy) │
//  │  ke hisaab se parameter lena PADEGA. Agar naam likha chhod dete to        │
//  │  compiler "-Wunused-parameter" warning deta. Naam ko /*comment*/ me daal  │
//  │  dene se: signature bhi sahi, warning bhi nahi, aur padhne wale ko saaf   │
//  │  dikhta ki "ye param jaan-boojh ke ignore hai". 📌 Classic C++ idiom.     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ SUCCESS return karta hai — par iska matlab "paisa mil gaya" NAHI, balki
//     "COD order accept ho gaya". Isi liye CheckoutService COD ke liye
//     PaymentStatus ko PENDING rakhta hai (SUCCESS nahi) — paisa abhi baaki hai.
//     (PaymentMethod.h me ye distinction explain kiya hai.)
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_STRATEGIES_CODPAYMENTSTRATEGY_H
#define ECOMMERCE_CART_CHECKOUT_LLD_STRATEGIES_CODPAYMENTSTRATEGY_H

#include <string>

#include "IPaymentStrategy.h"

namespace ecommerce_checkout_lld {

using namespace std;

class CodPaymentStrategy : public IPaymentStrategy {
public:
    // paymentInstrument COD me chahiye nahi — naam comment me (upar note).
    PaymentResult pay(double amount, const string &/*paymentInstrument*/) const override {
        if (amount <= 0) {
            return PaymentResult(PaymentStatus::FAILED, "", "Invalid amount");
        }
        const string txnId = "COD_" + to_string(static_cast<long long>(amount * 100));
        // SUCCESS = "order accepted", not "paisa mil gaya" (upar note).
        return PaymentResult(PaymentStatus::SUCCESS, txnId, "COD order accepted — pay on delivery");
    }
};

}  // namespace ecommerce_checkout_lld

#endif

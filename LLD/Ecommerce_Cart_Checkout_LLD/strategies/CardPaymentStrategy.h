// ============================================================================
//  strategies/CardPaymentStrategy.h  —  CONCRETE: credit card se payment
// ----------------------------------------------------------------------------
//  UPI se zyada validations, kyunki card decline ho sakta hai:
//     1. amount positive
//     2. card number kam se kam 4 digit
//     3. ⭐ "0000" pe khatam hone wale card -> BANK DECLINE (test scenario)
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ DECLINE simulate kaise hota hai (demo ke liye deterministic)          │
//  │                                                                          │
//  │     paymentInstrument.rfind("0000") == paymentInstrument.size() - 4       │
//  │                                                                          │
//  │  Matlab: "0000" ka AAKHRI occurrence exactly string ke end pe hai kya?    │
//  │  (rfind = last occurrence ka index; size()-4 = end se 4 char pehle.)      │
//  │  Agar haan -> card "0000" pe khatam -> decline.                           │
//  │                                                                          │
//  │  ⚠ `size() - 4` UNSIGNED subtraction hai — agar size < 4 hoti to ye       │
//  │  HUGE number me wrap kar jaata (underflow)! Isi liye UPAR wala            │
//  │  `size() < 4` check ZAROORI hai — wo pehle hi return kar deta hai.        │
//  │  Do checks ka ORDER yahan matter karta hai. 📌 Ye classic unsigned trap hai.│
//  │                                                                          │
//  │  Fayda: demo me "4111111111110000" hamesha fail karega aur                │
//  │  "4111111111114242" hamesha pass — deterministic testing (random nahi).  │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠ MOCK gateway — asli card processing (PCI, 3DS, tokenization) out of scope.
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_STRATEGIES_CARDPAYMENTSTRATEGY_H
#define ECOMMERCE_CART_CHECKOUT_LLD_STRATEGIES_CARDPAYMENTSTRATEGY_H

#include <string>

#include "IPaymentStrategy.h"

namespace ecommerce_checkout_lld {

using namespace std;

class CardPaymentStrategy : public IPaymentStrategy {
public:
    PaymentResult pay(double amount, const string &paymentInstrument) const override {
        if (amount <= 0) {
            return PaymentResult(PaymentStatus::FAILED, "", "Invalid amount");
        }
        // ⭐ Ye check neeche wale `size()-4` ko unsigned-underflow se bachata hai.
        if (paymentInstrument.size() < 4) {
            return PaymentResult(PaymentStatus::FAILED, "", "Invalid card number");
        }
        // "0000" pe khatam hone wale card = test decline (upar note).
        if (paymentInstrument.rfind("0000") == paymentInstrument.size() - 4) {
            return PaymentResult(PaymentStatus::FAILED, "", "Card declined by bank");
        }
        const string txnId = "CARD_TXN_" + to_string(static_cast<long long>(amount * 100));
        return PaymentResult(PaymentStatus::SUCCESS, txnId, "Paid via credit card");
    }
};

}  // namespace ecommerce_checkout_lld

#endif

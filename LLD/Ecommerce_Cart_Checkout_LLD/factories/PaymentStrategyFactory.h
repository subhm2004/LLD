// ============================================================================
//  factories/PaymentStrategyFactory.h  —  FACTORY PATTERN: enum se strategy banao
// ----------------------------------------------------------------------------
//  Ye ek hi jagah hai jahan "PaymentMethod -> kaunsi concrete strategy" ka faisla
//  hota hai. Baaki poore system me kahin bhi `if (method == UPI) ...` nahi likhna
//  padta — sab factory se strategy maang lete hain.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ FACTORY + STRATEGY ki JODI (dono saath kaam karte hain)               │
//  │                                                                          │
//  │     STRATEGY  = "kaam kaise karna hai" (UpiPayment / CardPayment / Cod)   │
//  │     FACTORY   = "kaunsa strategy object banana hai" (enum dekh ke)        │
//  │                                                                          │
//  │     PaymentService:                                                       │
//  │        auto strategy = PaymentStrategyFactory::create(method); // FACTORY │
//  │        return strategy->pay(amount, instrument);               // STRATEGY│
//  │                                                                          │
//  │  Fayda: naya payment method (WALLET) add karna ho to —                    │
//  │     1. enum me WALLET add                                                 │
//  │     2. WalletPaymentStrategy class banao                                  │
//  │     3. YAHAN ek case add                                                  │
//  │  ...aur PaymentService/CheckoutService/facade — kuch nahi badalta.       │
//  │  Creation ka knowledge EK jagah band (Single point of change).           │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `unique_ptr` return karta hai — caller ko ownership mil jaati hai aur
//     scope khatam hote hi strategy apne aap delete (koi manual cleanup nahi).
//
//  📌 `switch` ke baad wala `throw` defensive hai: agar kal koi naya enum value
//     add kare aur yahan case add karna BHOOL jaye, to chup-chaap galat behavior
//     ke bajaye saaf exception milega. (Compiler bhi -Wswitch se warn karta hai.)
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_FACTORIES_PAYMENTSTRATEGYFACTORY_H
#define ECOMMERCE_CART_CHECKOUT_LLD_FACTORIES_PAYMENTSTRATEGYFACTORY_H

#include <memory>
#include <stdexcept>

#include "../enums/PaymentMethod.h"
#include "../strategies/CardPaymentStrategy.h"
#include "../strategies/CodPaymentStrategy.h"
#include "../strategies/IPaymentStrategy.h"
#include "../strategies/UpiPaymentStrategy.h"

namespace ecommerce_checkout_lld {

using namespace std;

class PaymentStrategyFactory {
public:
    // Enum -> concrete strategy object (ownership caller ko).
    static unique_ptr<IPaymentStrategy> create(PaymentMethod method) {
        switch (method) {
            case PaymentMethod::UPI:
                return make_unique<UpiPaymentStrategy>();
            case PaymentMethod::CREDIT_CARD:
                return make_unique<CardPaymentStrategy>();
            case PaymentMethod::COD:
                return make_unique<CodPaymentStrategy>();
        }
        // Naya enum value add hua par case yahan nahi -> saaf error (upar note).
        throw invalid_argument("Unsupported payment method");
    }
};

}  // namespace ecommerce_checkout_lld

#endif

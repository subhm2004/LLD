// ============================================================================
//  services/PaymentService.h  —  Payment ka thin wrapper (Factory + Strategy jodta hai)
// ----------------------------------------------------------------------------
//  Poori class ek hi method ki hai. Kaam: method dekho -> Factory se sahi
//  Strategy banwao -> `pay()` chalao -> PaymentResult lauta do.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Itni patli class rakhne ka fayda kya?                                 │
//  │                                                                          │
//  │  CheckoutService ko ab `PaymentStrategyFactory` ya `IPaymentStrategy` ka  │
//  │  naam tak nahi pata — wo bas `paymentService_.processPayment(...)` bulata │
//  │  hai. Kal ko payment ka poora mechanism badle (real gateway SDK, retry    │
//  │  logic, 3D-Secure ka do-step flow, fraud check) to SIRF ye file badlegi.  │
//  │  Checkout ka orchestration code untouched. 📌 Yahi "seam" / boundary hai. │
//  │                                                                          │
//  │  ⚠ Aisa wrapper tab bekaar hota hai jab wo kuch add na kare AUR kabhi     │
//  │     badalne wala na ho. Yahan wo condition nahi — payment hamesha         │
//  │     badalta rehta hai (naye methods, naye gateways). Isliye justified.   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Strategy har call pe FRESH banti hai (member me store nahi). Isse:
//     - koi shared mutable state nahi (thread-safety ki tension kam),
//     - ek hi PaymentService se alag-alag methods handle ho jaate hain.
//     Strategies stateless hain to ye sasta hai.
//
//  📌 Method `const` hai — PaymentService ka apna koi state hai hi nahi
//     (stateless service). Isliye ise share karna bilkul safe hai.
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_PAYMENTSERVICE_H
#define ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_PAYMENTSERVICE_H

#include <string>

#include "../enums/PaymentMethod.h"
#include "../factories/PaymentStrategyFactory.h"
#include "../models/PaymentResult.h"

namespace ecommerce_checkout_lld {

using namespace std;

class PaymentService {
public:
    PaymentResult processPayment(PaymentMethod method,
                                 double amount,
                                 const string &paymentInstrument) const {
        // Factory: enum -> concrete strategy (unique_ptr, scope khatam hote hi free)
        auto strategy = PaymentStrategyFactory::create(method);
        // Strategy: asli payment logic (card / upi / cod) — yahan koi if-else nahi
        return strategy->pay(amount, paymentInstrument);
    }
};

}  // namespace ecommerce_checkout_lld

#endif

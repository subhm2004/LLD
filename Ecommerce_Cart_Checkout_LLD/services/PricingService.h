// ============================================================================
//  services/PricingService.h  —  Final bill ka math (subtotal -> payable)
// ----------------------------------------------------------------------------
//  Ek hi jagah pe poora price calculation. Formula:
//
//      afterDiscount = subtotal - min(discount, subtotal)
//      shipping      = (afterDiscount >= 999) ? 0 : 49
//      payable       = afterDiscount + shipping
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ `min(discount, subtotal)` — ye chhoti si line bahut kuch bachati hai │
//  │                                                                          │
//  │  Socho: cart ₹800 ka hai, coupon FLAT1000 ka. Bina clamp ke:              │
//  │      afterDiscount = 800 - 1000 = -200   -> payable = -151 😱             │
//  │  Yaani DUKAAN customer ko paise degi! Clamp lagne se discount 800 pe cap  │
//  │  ho jaata hai -> afterDiscount = 0 -> payable = 49 (sirf shipping).       │
//  │                                                                          │
//  │  📌 DEFENSE IN DEPTH: FlatDiscountStrategy me bhi clamp hai. Do jagah     │
//  │     kyun? Kyunki discount kahin aur se bhi aa sakta hai (loyalty points,  │
//  │     cashback, do coupons ka jod). Paise wale code me FINAL gate hamesha   │
//  │     apna check khud karta hai — upstream pe bharosa nahi karta.          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Shipping DISCOUNT KE BAAD wali amount pe decide hoti hai (subtotal pe nahi).
//     ₹1050 cart + ₹100 coupon -> afterDiscount 950 -> free shipping NAHI milti.
//     Ye ek business POLICY decision hai (dono tarah ho sakta tha) — yahan ek hi
//     line me likha hai, isliye policy badalni ho to ek jagah badlo.
//
//  ⭐ Thresholds CONSTRUCTOR se aate hain (hard-coded nahi). Sale ke din
//     `PricingService(499.0, 0.0)` bana ke free-shipping sasti kar sakte ho —
//     baaki koi file badalni nahi padti. (Configuration over modification.)
//
//  📌 `PriceBreakup` struct return karta hai — sirf `payable` nahi. Kyunki UI ko
//     poora breakup dikhana hota hai aur Order me har component alag store hota
//     hai (Order.h ka note). Ek call, saara answer.
//
//  ⚠ `double` in money — sikhne ke liye theek, production me nahi (0.1 + 0.2 !=
//     0.3 wala float issue). Real system me paise ko INTEGER PAISE me rakhte hain
//     (₹49.00 -> 4900) ya decimal/BigDecimal type use karte hain.
// ============================================================================
#ifndef ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_PRICINGSERVICE_H
#define ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_PRICINGSERVICE_H

#include <algorithm>

namespace ecommerce_checkout_lld {

using namespace std;

// Bill ka poora breakup ek hi jagah (Order me as-is store hota hai).
struct PriceBreakup {
    double subtotal{0.0};      // items ka jod
    double discount{0.0};      // clamp ho chuka discount
    double shippingFee{0.0};   // 0 ya flat fee
    double payableAmount{0.0}; // jo actually charge hoga
};

class PricingService {
public:
    // ⭐ Policy numbers injectable (default: 999 pe free, warna 49).
    explicit PricingService(double freeShippingThreshold = 999.0, double flatShippingFee = 49.0)
        : freeShippingThreshold_(freeShippingThreshold), flatShippingFee_(flatShippingFee) {}

    PriceBreakup calculate(double subtotal, double discount) const {
        PriceBreakup breakup;
        breakup.subtotal = subtotal;
        breakup.discount = min(discount, subtotal); // ⭐⭐ negative payable ka guard
        const double afterDiscount = subtotal - breakup.discount;

        // ⭐ Shipping DISCOUNT KE BAAD wali amount pe (business policy — upar note).
        breakup.shippingFee = afterDiscount >= freeShippingThreshold_ ? 0.0 : flatShippingFee_;
        breakup.payableAmount = afterDiscount + breakup.shippingFee;
        return breakup;
    }

private:
    double freeShippingThreshold_{999.0}; // itne ke upar shipping free
    double flatShippingFee_{49.0};        // warna itna charge
};

}  // namespace ecommerce_checkout_lld

#endif

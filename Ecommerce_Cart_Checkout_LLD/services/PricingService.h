#ifndef ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_PRICINGSERVICE_H
#define ECOMMERCE_CART_CHECKOUT_LLD_SERVICES_PRICINGSERVICE_H

#include <bits/stdc++.h>

namespace ecommerce_checkout_lld {

using namespace std;

struct PriceBreakup {
    double subtotal{0.0};
    double discount{0.0};
    double shippingFee{0.0};
    double payableAmount{0.0};
};

class PricingService {
public:
    explicit PricingService(double freeShippingThreshold = 999.0, double flatShippingFee = 49.0)
        : freeShippingThreshold_(freeShippingThreshold), flatShippingFee_(flatShippingFee) {}

    PriceBreakup calculate(double subtotal, double discount) const {
        PriceBreakup breakup;
        breakup.subtotal = subtotal;
        breakup.discount = min(discount, subtotal);
        const double afterDiscount = subtotal - breakup.discount;
        breakup.shippingFee = afterDiscount >= freeShippingThreshold_ ? 0.0 : flatShippingFee_;
        breakup.payableAmount = afterDiscount + breakup.shippingFee;
        return breakup;
    }

private:
    double freeShippingThreshold_{999.0};
    double flatShippingFee_{49.0};
};

}  // namespace ecommerce_checkout_lld

#endif

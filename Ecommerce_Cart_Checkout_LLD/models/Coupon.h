#ifndef ECOMMERCE_CART_CHECKOUT_LLD_MODELS_COUPON_H
#define ECOMMERCE_CART_CHECKOUT_LLD_MODELS_COUPON_H

#include <bits/stdc++.h>
#include <memory>
#include <string>

#include "../interfaces/IDiscountStrategy.h"

namespace ecommerce_checkout_lld {

using namespace std;

class Coupon {
public:
    Coupon(string code,
           double minCartValue,
           std::unique_ptr<IDiscountStrategy> strategy,
           int expiryDay)
        : code_(std::move(code)),
          minCartValue_(minCartValue),
          strategy_(std::move(strategy)),
          expiryDay_(expiryDay) {}

    const string &getCode() const { return code_; }
    double getMinCartValue() const { return minCartValue_; }
    int getExpiryDay() const { return expiryDay_; }

    bool isValidFor(double cartSubtotal, int currentDay) const {
        return currentDay <= expiryDay_ && cartSubtotal >= minCartValue_;
    }

    double calculateDiscount(double cartSubtotal) const {
        if (!strategy_) {
            return 0.0;
        }
        return strategy_->calculate(cartSubtotal);
    }

private:
    string code_;
    double minCartValue_{0.0};
    std::unique_ptr<IDiscountStrategy> strategy_;
    int expiryDay_{0};
};

}  // namespace ecommerce_checkout_lld

#endif

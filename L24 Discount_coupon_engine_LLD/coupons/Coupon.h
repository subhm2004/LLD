#ifndef DISCOUNT_COUPON_LLD_COUPONS_COUPON_H
#define DISCOUNT_COUPON_LLD_COUPONS_COUPON_H

#include <iostream>
#include <string>

#include "../models/Cart.h"

namespace discount_coupon_lld {

class Coupon {
    Coupon *next = nullptr;

public:
    virtual ~Coupon() {
        delete next;
    }

    void setNext(Coupon *nxt) { next = nxt; }
    Coupon *getNext() const { return next; }

    void applyDiscount(Cart *cart) {
        if (isApplicable(cart)) {
            double discount = getDiscount(cart);
            cart->applyDiscount(discount);
            std::cout << name() << " applied: " << discount << '\n';
            if (!isCombinable()) {
                return;
            }
        }
        if (next) {
            next->applyDiscount(cart);
        }
    }

    virtual bool isApplicable(Cart *cart) = 0;
    virtual double getDiscount(Cart *cart) = 0;
    virtual bool isCombinable() { return true; }
    virtual std::string name() = 0;
};

}

#endif

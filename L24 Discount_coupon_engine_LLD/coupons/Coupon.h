#ifndef DISCOUNT_COUPON_LLD_COUPONS_COUPON_H
#define DISCOUNT_COUPON_LLD_COUPONS_COUPON_H

#include <iostream>
#include <string>

#include "../models/Cart.h"

namespace discount_coupon_lld {

// -----------------------------------------------------------------------------
// Coupon base class (Chain of Responsibility)
// Kya karta hai:
// - Coupon chain ka generic behavior define karta hai.
// - Concrete coupon ko sirf 3 cheezein batani hoti hain:
//   1) applicability rule
//   2) discount amount
//   3) name
// -----------------------------------------------------------------------------
class Coupon {
    Coupon *next = nullptr;

public:
    // Recursive cleanup:
    // head delete karne par poori chain release ho jati hai.
    virtual ~Coupon() {
        delete next;
    }

    void setNext(Coupon *nxt) { next = nxt; }
    Coupon *getNext() const { return next; }

    // Template-style chain step:
    // - applicable ho to discount apply karo
    // - non-combinable ho to yahin stop
    // - warna next coupon evaluate karo
    void applyDiscount(Cart *cart) {
        if (isApplicable(cart)) {
            double discount = getDiscount(cart);
            cart->applyDiscount(discount);
            std::cout << name() << " applied: " << discount << '\n';
            if (!isCombinable()) {
                std::cout << name() << " is exclusive — stopping coupon chain.\n";
                return;
            }
        }
        if (next) {
            next->applyDiscount(cart);
        }
    }

    // Concrete coupon ka contract
    virtual bool isApplicable(Cart *cart) = 0;
    virtual double getDiscount(Cart *cart) = 0;

    // Default behavior: stackable
    // Exclusive coupon yeh method override karke false return kar sakta hai.
    virtual bool isCombinable() { return true; }
    virtual std::string name() = 0;
};

}

#endif

// ============================================================================
//  coupons/Coupon.h — CHAIN OF RESPONSIBILITY ka base class (system ka dil ❤️)
// ----------------------------------------------------------------------------
//  Chain of Responsibility = "request ko handlers ki EK CHAIN se guzaro —
//  har handler khud decide kare: main handle karu? aage bheju? ya rok du?"
//
//  Yahan: Cart (request) coupons ki chain se guzarta hai. Har coupon:
//    1. isApplicable(cart)? -> nahi: SKIP karke next ko de do
//    2. haan: discount lagao
//    3. isCombinable()? -> false: chain YAHIN STOP (exclusive coupon!)
//    4. warna next coupon ko pass karo (discounts STACK hote hain)
//
//  Concrete coupon ko sirf 3 cheezein batani hoti hain (contract):
//    - isApplicable()  : "main is cart pe lagta hoon?"    (eligibility rule)
//    - getDiscount()   : "kitna discount?"                (strategy se math)
//    - name()          : "mera naam kya hai?"             (logging/listing)
//  Chain chalane ka SAARA logic (apply/skip/stop/next) BASE me ek jagah hai —
//  ye Template Method jaisa flavour hai: skeleton base me, details derived me.
// ============================================================================
#ifndef DISCOUNT_COUPON_LLD_COUPONS_COUPON_H
#define DISCOUNT_COUPON_LLD_COUPONS_COUPON_H

#include <iostream>
#include <string>

#include "../models/Cart.h"

namespace discount_coupon_lld {

// -----------------------------------------------------------------------------
// Coupon base class (Chain of Responsibility ka "Handler")
// -----------------------------------------------------------------------------
class Coupon {
    // Chain ka agla link — nullptr matlab ye aakhri coupon hai.
    // Ye pointer hi coupons ko linked-list jaisi chain me jodta hai.
    Coupon *next = nullptr;

public:
    // RECURSIVE CLEANUP ka trick: har coupon apne NEXT ko delete karta hai,
    // wo apne next ko... to sirf HEAD delete karo -> poori chain ek saath
    // release! (Domino effect 🁢🁢🁢)
    virtual ~Coupon() {
        delete next;
    }

    void setNext(Coupon *nxt) { next = nxt; }
    Coupon *getNext() const { return next; }

    // ------------------- CHAIN KA ENGINE (sabse important method) -------------------
    // Ye method BASE me hai, virtual NAHI hai — chain chalane ka tareeka
    // sab coupons ke liye SAME hai, sirf rules (isApplicable/getDiscount)
    // alag hain. Flow:
    //   applicable? -> discount lagao -> combinable nahi? STOP : next->apply
    void applyDiscount(Cart *cart) {
        if (isApplicable(cart)) {
            double discount = getDiscount(cart);      // math STRATEGY se aata hai
            cart->applyDiscount(discount);            // cart ka currentTotal ghata
            std::cout << name() << " applied: " << discount << '\n';
            if (!isCombinable()) {
                // EXCLUSIVE coupon (jaise BankingCoupon) — iske baad koi
                // aur discount stack nahi ho sakta, chain yahin khatam!
                std::cout << name() << " is exclusive — stopping coupon chain.\n";
                return;
            }
        }
        // Applicable nahi tha (skip) YA combinable tha — dono case me
        // baaki chain ko mauka do.
        if (next) {
            next->applyDiscount(cart);
        }
    }

    // ------------------- CONCRETE COUPON KA CONTRACT -------------------
    // Har concrete coupon (Seasonal/Loyalty/Bulk/Banking) ko ye dena HOGA:
    virtual bool isApplicable(Cart *cart) = 0;  // eligibility rule
    virtual double getDiscount(Cart *cart) = 0; // discount amount (via strategy)

    // Default: STACKABLE (chain aage badhegi). Exclusive coupon (Banking)
    // isse override karke false karta hai -> chain wahin ruk jaati hai.
    virtual bool isCombinable() { return true; }
    virtual std::string name() = 0;
};

}

#endif

// ============================================================================
//  coupons/BankingCoupon.h — Concrete Coupon #4: bank offer (EXCLUSIVE! 🛑)
// ----------------------------------------------------------------------------
//  "ABC Bank card se pay karo, 15% off (max Rs 500), min spend 2000."
//  Ye sabse FEATURE-RICH coupon hai — teeno khaas cheezein ek saath:
//    1. DOUBLE eligibility: bank match AND original total >= min spend
//    2. PERCENT_WITH_CAP strategy — 15% to milega par Rs 500 se zyada nahi
//       (bank apna nuksan cap karta hai — real life jaisa!)
//    3. EXCLUSIVE: isCombinable() = false override — ye lagne ke baad
//       chain STOP! Iske aage koi coupon stack nahi hota. Isi liye main.cpp
//       me ise chain me SABSE AAKHIR me register kiya hai — pehle daal dete
//       to baaki combinable coupons ko mauka hi nahi milta!
// ============================================================================
#ifndef DISCOUNT_COUPON_LLD_COUPONS_BANKINGCOUPON_H
#define DISCOUNT_COUPON_LLD_COUPONS_BANKINGCOUPON_H

#include "../enums/StrategyType.h"
#include "../factories/DiscountStrategyManager.h"
#include "Coupon.h"

namespace discount_coupon_lld {

// -----------------------------------------------------------------------------
// BankingCoupon coupon
// Kya karta hai:
// - Specific bank + minimum spend rule par based percent discount deta hai.
// - Discount capped hota hai (max limit).
// - Exclusive behavior: apply hone ke baad chain stop.
// -----------------------------------------------------------------------------
class BankingCoupon : public Coupon {
    std::string bank;
    double minSpend;
    double percent;
    double offCap;
    IDiscountStrategy *strategy = nullptr;

public:
    BankingCoupon(const std::string &bankName, double minSpendAmount, double discountPercent, double discountCap)
        : bank(bankName), minSpend(minSpendAmount), percent(discountPercent), offCap(discountCap) {
        // Banking coupon math = percent with cap
        strategy = DiscountStrategyManager::getInstance().createStrategy(StrategyType::PERCENT_WITH_CAP, percent,
                                                                         offCap);
    }

    ~BankingCoupon() override { delete strategy; }

    // Eligibility:
    // 1) selected payment bank match ho
    // 2) original total minimum spend se bada/equal ho
    bool isApplicable(Cart *cart) override {
        return cart->getPaymentBank() == bank && cart->getOriginalTotal() >= minSpend;
    }

    // Discount current total par compute hota hai.
    double getDiscount(Cart *cart) override { return strategy->calculate(cart->getCurrentTotal()); }

    // Exclusive coupon:
    // true stacking ke bajay chain yahin stop karni hai.
    bool isCombinable() override { return false; }

    std::string name() override {
        return bank + " Bank " + std::to_string(static_cast<int>(percent)) + "% off upto " +
               std::to_string(static_cast<int>(offCap)) + " (exclusive)";
    }
};

}

#endif

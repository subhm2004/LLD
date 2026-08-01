// ============================================================================
//  factories/DiscountStrategyManager.h — SIMPLE FACTORY + SINGLETON
// ----------------------------------------------------------------------------
//  Ye L9 wala Simple Factory hai, real project me use hota hua!
//    - Coupons ko strategy chahiye par unhe CONCRETE class names
//      (FlatDiscountStrategy...) nahi pata hone chahiye — wo bas enum
//      (StrategyType::FLAT) bolte hain, factory sahi object bana deti hai.
//    - Singleton isliye: factory STATELESS hai, iske do instances banane
//      ka koi matlab hi nahi — ek hi kaafi hai.
//    - Copy constructor + operator= DELETE kiye hain — Singleton ko galti
//      se copy karke "do instances" banne ka rasta hi band! (L9 se ek
//      kadam aage — interview me ye detail bolo.)
//
//  Simple Factory ka known trade-off yahan bhi hai: naya StrategyType aaya
//  to is switch me case add karna padega (OCP weak) — par creation EK
//  jagah centralized hai, har coupon me `new` bikhra nahi.
// ============================================================================
#ifndef DISCOUNT_COUPON_LLD_FACTORIES_DISCOUNTSTRATEGYMANAGER_H
#define DISCOUNT_COUPON_LLD_FACTORIES_DISCOUNTSTRATEGYMANAGER_H

#include "../enums/StrategyType.h"
#include "../strategies/FlatDiscountStrategy.h"
#include "../strategies/PercentageDiscountStrategy.h"
#include "../strategies/PercentageWithCapStrategy.h"

namespace discount_coupon_lld {

// -----------------------------------------------------------------------------
// DiscountStrategyManager (Singleton + Factory)
// Kya karta hai:
// - Strategy object creation ka single centralized point.
// - Client/coupon classes ko concrete strategy class names nahi pata hone chahiye.
// -----------------------------------------------------------------------------
class DiscountStrategyManager {
    DiscountStrategyManager() = default;
    DiscountStrategyManager(const DiscountStrategyManager &) = delete;
    DiscountStrategyManager &operator=(const DiscountStrategyManager &) = delete;

public:
    // Meyers singleton style instance (C++11+ thread-safe init)
    static DiscountStrategyManager &getInstance() {
        static DiscountStrategyManager instance;
        return instance;
    }

    // type + params ke basis par concrete strategy object create karta hai.
    // Ownership caller ke paas jata hai (caller must delete).
    IDiscountStrategy *createStrategy(StrategyType type, double param1, double param2 = 0.0) const {
        switch (type) {
        case StrategyType::FLAT:
            return new FlatDiscountStrategy(param1);
        case StrategyType::PERCENT:
            return new PercentageDiscountStrategy(param1);
        case StrategyType::PERCENT_WITH_CAP:
            return new PercentageWithCapStrategy(param1, param2);
        }
        return nullptr;
    }
};

}

#endif

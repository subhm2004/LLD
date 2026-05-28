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

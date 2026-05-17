#ifndef DISCOUNT_COUPON_LLD_FACTORIES_DISCOUNTSTRATEGYMANAGER_H
#define DISCOUNT_COUPON_LLD_FACTORIES_DISCOUNTSTRATEGYMANAGER_H

#include "../enums/StrategyType.h"
#include "../strategies/FlatDiscountStrategy.h"
#include "../strategies/PercentageDiscountStrategy.h"
#include "../strategies/PercentageWithCapStrategy.h"

namespace discount_coupon_lld {

class DiscountStrategyManager {
    DiscountStrategyManager() = default;
    DiscountStrategyManager(const DiscountStrategyManager &) = delete;
    DiscountStrategyManager &operator=(const DiscountStrategyManager &) = delete;

public:
    static DiscountStrategyManager &getInstance() {
        static DiscountStrategyManager instance;
        return instance;
    }

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

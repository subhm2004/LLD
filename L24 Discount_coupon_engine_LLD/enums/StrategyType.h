// enums/StrategyType.h — Discount strategy types: FLAT / PERCENT / PERCENT_WITH_CAP.
// DiscountStrategyManager isse sahi strategy object deta hai.
#ifndef DISCOUNT_COUPON_LLD_ENUMS_STRATEGYTYPE_H
#define DISCOUNT_COUPON_LLD_ENUMS_STRATEGYTYPE_H

namespace discount_coupon_lld {

// -----------------------------------------------------------------------------
// StrategyType
// Kya karta hai:
// - DiscountStrategyManager ko batata hai kaunsa discount math strategy object
//   create karna hai.
// -----------------------------------------------------------------------------
enum class StrategyType { FLAT, PERCENT, PERCENT_WITH_CAP };

}

#endif

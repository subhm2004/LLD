// ============================================================================
//  enums/StrategyType.h — Strategy types ka enum (factory ki "menu card" 📋)
// ----------------------------------------------------------------------------
//  Coupons factory ko STRING nahi, ye type-safe enum dete hain — typo
//  ("falt"?) compile-time pe hi pakda jaayega, runtime pe nahi.
//  `enum class` (plain enum nahi) — values scoped rehti hain
//  (StrategyType::FLAT) aur int me chupke se convert nahi hoti. Modern C++!
// ============================================================================
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

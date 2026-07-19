// ============================================================================
//  strategies/PricingStrategy.h — Base pricing ka STRATEGY interface (L8) [NAYA]
// ----------------------------------------------------------------------------
//  Base rental amount kaise calculate ho — ye SWAP-ABLE policy hai:
//    DailyPricingStrategy   -> dailyRate × days (simple)
//    (future) WeekendSurge  -> weekend pe zyada
//    (future) SeasonalPricing, LoyaltyDiscount...
//
//  PricingService (context) ise hold karta hai. Naya pricing model = nayi
//  strategy class, baaki system untouched. Ye Decorator (add-ons) se ALAG
//  hai — Strategy BASE ko price karta, Decorator uspe EXTRAS jodta.
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_STRATEGIES_PRICINGSTRATEGY_H
#define CAR_RENTAL_SYSTEM_LLD_STRATEGIES_PRICINGSTRATEGY_H

namespace car_rental_lld {

class PricingStrategy {
public:
    virtual ~PricingStrategy() = default;
    virtual double calculateBaseAmount(double dailyRate, int days) const = 0;
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_STRATEGIES_PRICINGSTRATEGY_H

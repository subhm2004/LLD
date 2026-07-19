// ============================================================================
//  strategies/DailyPricingStrategy.h — Concrete Strategy: simple daily [NAYA]
// ----------------------------------------------------------------------------
//  Base amount = dailyRate × days. Sabse seedha pricing model.
//  (Kal WeekendSurge/Seasonal aa sakti hai — bas nayi strategy class,
//   PricingService me inject karo.)
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_STRATEGIES_DAILYPRICINGSTRATEGY_H
#define CAR_RENTAL_SYSTEM_LLD_STRATEGIES_DAILYPRICINGSTRATEGY_H

#include "PricingStrategy.h"

namespace car_rental_lld {

class DailyPricingStrategy : public PricingStrategy {
public:
    double calculateBaseAmount(double dailyRate, int days) const override {
        return dailyRate * days;
    }
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_STRATEGIES_DAILYPRICINGSTRATEGY_H

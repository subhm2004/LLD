// ============================================================================
//  services/PricingService.h  —  Strategy Pattern ka CONTEXT (pricing holder)
// ----------------------------------------------------------------------------
//  Ye service ek pricing strategy ko HOLD karti hai aur pricing calls ko usko
//  delegate karti hai. Strategy runtime pe swap ho sakti hai (setStrategy).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ CONTEXT — strategy ko rakhta hai, khud pricing logic nahi janta       │
//  │                                                                          │
//  │  calculateStayAmount() bas `strategy_->calculateTotal(...)` bulata hai.   │
//  │  Kaunsa formula chalega (flat / weekend surge), wo `strategy_` decide     │
//  │  karti hai. Facade (useWeekendPricing) setStrategy se ise badal deta.    │
//  │                                                                          │
//  │  Default: StandardPricingStrategy (flat rate) — constructor me set.       │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `unique_ptr<IPricingStrategy>` — service strategy ko OWN karti hai (ek hi
//     maalik). setStrategy naya assign kare to purani apne aap delete (unique_ptr).
//     Aur null strategy reject (guard) — taaki calculate pe crash na ho.
//
//  📌 unique_ptr member ki wajah se PricingService (aur use rakhne wala system)
//     non-copyable ban jaata — jo accha hai (galti se copy = do owner = double
//     free se bacha lete hain).
// ============================================================================
#ifndef OYO_HOTEL_BOOKING_LLD_SERVICES_PRICINGSERVICE_H
#define OYO_HOTEL_BOOKING_LLD_SERVICES_PRICINGSERVICE_H

#include <memory>
#include <stdexcept>
#include <utility>

#include "../strategies/IPricingStrategy.h"
#include "../strategies/StandardPricingStrategy.h"

namespace oyo_hotel_lld {

using namespace std;

class PricingService {
public:
    PricingService() : strategy_(make_unique<StandardPricingStrategy>()) {} // default: flat

    // Runtime pe pricing strategy badlo (null reject).
    void setStrategy(unique_ptr<IPricingStrategy> strategy) {
        if (!strategy) {
            throw invalid_argument("Pricing strategy cannot be null");
        }
        strategy_ = std::move(strategy); // purani apne aap delete
    }

    // Delegate to current strategy — context ko formula ka pata nahi.
    double calculateStayAmount(double pricePerNight, int checkInDay, int checkOutDay) const {
        return strategy_->calculateTotal(pricePerNight, checkInDay, checkOutDay);
    }

private:
    unique_ptr<IPricingStrategy> strategy_; // current pricing strategy (owned, swappable)
};

}  // namespace oyo_hotel_lld

#endif

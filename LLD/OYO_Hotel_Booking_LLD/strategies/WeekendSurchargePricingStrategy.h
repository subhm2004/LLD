// ============================================================================
//  strategies/WeekendSurchargePricingStrategy.h  —  CONCRETE STRATEGY: weekend surge
// ----------------------------------------------------------------------------
//  Weekend (Sat/Sun) waali raaton pe zyada daam (default 1.25x). Baaki din normal.
//  Isliye ye NIGHT-BY-NIGHT loop karke total banata hai (StandardPricing ki tarah
//  ek simple multiply nahi).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ WEEKEND kaise decide hota (simplified modulo calendar)                │
//  │                                                                          │
//  │     isWeekend = (day % 7 == 6 || day % 7 == 0)                            │
//  │                                                                          │
//  │  Yaani har 7-din ke cycle me 2 din weekend (jaise day 6,7,13,14,20,21).   │
//  │  Real calendar ki jagah ye simple modulo — kyunki dates yahan int "days"  │
//  │  hain (asli weekday nahi). Interview-scope me kaafi.                      │
//  │                                                                          │
//  │  Example (1200/night, days 12->15 = nights 12,13,14):                     │
//  │     day 12: 12%7=5 (weekday) -> 1200                                      │
//  │     day 13: 13%7=6 (weekend) -> 1200 x 1.25 = 1500                        │
//  │     day 14: 14%7=0 (weekend) -> 1500                                      │
//  │     total = 1200 + 1500 + 1500 = 4200                                     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Multiplier CONSTRUCTOR se aata hai (default 1.25) — configurable. Kal ko
//     1.5x chahiye to `WeekendSurchargePricingStrategy(1.5)`. Strategy khud
//     parameterizable hai (ek hi class, alag surge levels).
//
//  📌 Notice: ye StandardPricing SE ALAG behavior deta hai bina IPricingStrategy
//     ya PricingService ko chhue — bas ek nayi class. Yahi Strategy ka poora point.
// ============================================================================
#ifndef OYO_HOTEL_BOOKING_LLD_STRATEGIES_WEEKENDSURCHARGEPRICINGSTRATEGY_H
#define OYO_HOTEL_BOOKING_LLD_STRATEGIES_WEEKENDSURCHARGEPRICINGSTRATEGY_H

#include <stdexcept>

#include "IPricingStrategy.h"

namespace oyo_hotel_lld {

using namespace std;

class WeekendSurchargePricingStrategy : public IPricingStrategy {
public:
    explicit WeekendSurchargePricingStrategy(double weekendMultiplier = 1.25)
        : weekendMultiplier_(weekendMultiplier) {}

    double calculateTotal(double pricePerNight, int checkInDay, int checkOutDay) const override {
        if (checkOutDay <= checkInDay) {
            throw invalid_argument("checkOutDay must be after checkInDay");
        }
        double total = 0.0;
        // ⭐ Har raat alag se — weekend hai to multiplier lagao, warna normal.
        for (int day = checkInDay; day < checkOutDay; ++day) { // half-open [in, out)
            const bool isWeekend = (day % 7 == 6 || day % 7 == 0);
            total += pricePerNight * (isWeekend ? weekendMultiplier_ : 1.0);
        }
        return total;
    }

private:
    double weekendMultiplier_; // weekend raat pe kitna guna (default 1.25)
};

}  // namespace oyo_hotel_lld

#endif

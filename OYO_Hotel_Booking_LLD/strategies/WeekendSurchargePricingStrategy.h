#ifndef OYO_HOTEL_BOOKING_LLD_STRATEGIES_WEEKENDSURCHARGEPRICINGSTRATEGY_H
#define OYO_HOTEL_BOOKING_LLD_STRATEGIES_WEEKENDSURCHARGEPRICINGSTRATEGY_H

#include <bits/stdc++.h>

#include "IPricingStrategy.h"

namespace oyo_hotel_lld {

using namespace std;

/** Days 6,7,13,14,... treated as weekend (simplified modulo calendar). */
class WeekendSurchargePricingStrategy : public IPricingStrategy {
public:
    explicit WeekendSurchargePricingStrategy(double weekendMultiplier = 1.25)
        : weekendMultiplier_(weekendMultiplier) {}

    double calculateTotal(double pricePerNight, int checkInDay, int checkOutDay) const override {
        if (checkOutDay <= checkInDay) {
            throw invalid_argument("checkOutDay must be after checkInDay");
        }
        double total = 0.0;
        for (int day = checkInDay; day < checkOutDay; ++day) {
            const bool isWeekend = (day % 7 == 6 || day % 7 == 0);
            total += pricePerNight * (isWeekend ? weekendMultiplier_ : 1.0);
        }
        return total;
    }

private:
    double weekendMultiplier_;
};

}  // namespace oyo_hotel_lld

#endif

#ifndef OYO_HOTEL_BOOKING_LLD_STRATEGIES_STANDARDPRICINGSTRATEGY_H
#define OYO_HOTEL_BOOKING_LLD_STRATEGIES_STANDARDPRICINGSTRATEGY_H

#include <bits/stdc++.h>

#include "IPricingStrategy.h"

namespace oyo_hotel_lld {

using namespace std;

class StandardPricingStrategy : public IPricingStrategy {
public:
    double calculateTotal(double pricePerNight, int checkInDay, int checkOutDay) const override {
        if (checkOutDay <= checkInDay) {
            throw invalid_argument("checkOutDay must be after checkInDay");
        }
        return pricePerNight * static_cast<double>(checkOutDay - checkInDay);
    }
};

}  // namespace oyo_hotel_lld

#endif

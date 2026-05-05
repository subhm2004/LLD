#ifndef PARKING_LOT_SYSTEM_LLD_STRATEGIES_HOURLYPRICINGSTRATEGY_H
#define PARKING_LOT_SYSTEM_LLD_STRATEGIES_HOURLYPRICINGSTRATEGY_H

#include <bits/stdc++.h>

#include "PricingStrategy.h"

using namespace std;

namespace parking_lot_lld {

class HourlyPricingStrategy : public PricingStrategy {
public:
    double calculateFee(VehicleType type, long long entryTime, long long exitTime) const override {
        if (exitTime < entryTime) {
            throw runtime_error("Exit time cannot be less than entry time");
        }

        const long long totalSeconds = exitTime - entryTime;
        int hours = static_cast<int>((totalSeconds + 3599) / 3600);
        if (hours == 0) {
            hours = 1;
        }

        double rate = 20.0;
        if (type == VehicleType::CAR) {
            rate = 40.0;
        } else if (type == VehicleType::TRUCK) {
            rate = 80.0;
        }
        return rate * hours;
    }
};

} // namespace parking_lot_lld

#endif // PARKING_LOT_SYSTEM_LLD_STRATEGIES_HOURLYPRICINGSTRATEGY_H

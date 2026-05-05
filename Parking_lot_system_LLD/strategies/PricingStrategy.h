#ifndef PARKING_LOT_SYSTEM_LLD_STRATEGIES_PRICINGSTRATEGY_H
#define PARKING_LOT_SYSTEM_LLD_STRATEGIES_PRICINGSTRATEGY_H

#include <bits/stdc++.h>

#include "../enums/VehicleType.h"

using namespace std;

namespace parking_lot_lld {

class PricingStrategy {
public:
    virtual ~PricingStrategy() = default;
    virtual double calculateFee(VehicleType type, long long entryTime, long long exitTime) const = 0;
};

} // namespace parking_lot_lld

#endif // PARKING_LOT_SYSTEM_LLD_STRATEGIES_PRICINGSTRATEGY_H

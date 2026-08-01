// ============================================================================
//  strategies/HourlyPricingStrategy.h — Concrete Strategy: HOURLY rates [req #10]
// ----------------------------------------------------------------------------
//  Requirement #10: charges duration ke hisaab se, HOURLY apply hote.
//  Formula: fee = perHourRate(vehicleType) × hours-parked (round-up).
//
//  ⭐ CEILING (round-up) trick: 1 hour 1 minute = 2 hours ka charge!
//  Parking me part-hour bhi full-hour count hota (real-world rule). Isliye
//  (totalSeconds + 3599) / 3600 — ye ceiling division hai (agla poora ghanta).
//  Aur minimum 1 hour (0 hour = free nahi, kam se kam 1 ghanta charge).
//
//  RATES (vehicle type ke hisaab se — bada vehicle = zyada charge):
//    MOTORCYCLE -> Rs 20/hr   (sabse sasta)
//    CAR        -> Rs 40/hr
//    VAN        -> Rs 60/hr
//    TRUCK      -> Rs 80/hr   (sabse mehnga — sabse zyada jagah leta)
//  (VAN naya add kiya requirement ke hisaab se.)
// ============================================================================
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

        // Duration ko HOURS me (round-up). +3599 = ceiling division ka trick.
        const long long totalSeconds = exitTime - entryTime;
        int hours = static_cast<int>((totalSeconds + 3599) / 3600);
        if (hours == 0) {
            hours = 1;  // minimum 1 ghanta charge (even for 0-second stay)
        }

        // Per-hour rate vehicle type ke hisaab se
        double rate = 20.0;  // default: MOTORCYCLE
        if (type == VehicleType::CAR) {
            rate = 40.0;
        } else if (type == VehicleType::VAN) {
            rate = 60.0;
        } else if (type == VehicleType::TRUCK) {
            rate = 80.0;
        }
        return rate * hours;
    }
};

} // namespace parking_lot_lld

#endif // PARKING_LOT_SYSTEM_LLD_STRATEGIES_HOURLYPRICINGSTRATEGY_H

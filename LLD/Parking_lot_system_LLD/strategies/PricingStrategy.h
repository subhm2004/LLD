// ============================================================================
//  strategies/PricingStrategy.h — Pricing ka STRATEGY interface (L8) [req #10]
// ----------------------------------------------------------------------------
//  Fee calculate karne ka tareeka SWAP-ABLE hona chahiye (non-functional req).
//  Abhi HourlyPricingStrategy hai (duration × rate), par kal DailyPricing,
//  FlatRate, WeekendSurge... koi bhi aa sakti hai — bas is interface ko
//  implement karo, ParkingLot untouched. Yahi Strategy pattern.
//
//  calculateFee ko milta hai: vehicle type (rate isse depend karta —
//  truck > car > bike), entryTime aur exitTime (duration = exit - entry).
//  ParkingLot exit pe isse fee nikaalta.
// ============================================================================
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

// ============================================================================
//  strategies/StandardPricingStrategy.h  —  CONCRETE STRATEGY: flat rate
// ----------------------------------------------------------------------------
//  Sabse simple pricing: total = per-night price x number of nights. Har raat ka
//  same daam, koi surcharge/discount nahi. Ye DEFAULT strategy hai (PricingService
//  isi se shuru hoti).
//
//     total = pricePerNight x (checkOutDay - checkInDay)
//     jaise: 1800/night x 2 nights = 3600
//
//  ⭐ Validation: checkOut, checkIn ke baad hona chahiye (warna nights negative/0).
//     Galat range -> invalid_argument (fail-fast). Ye check har strategy me hai
//     (defensive) — bhale facade bhi validate karta hai (defense in depth).
// ============================================================================
#ifndef OYO_HOTEL_BOOKING_LLD_STRATEGIES_STANDARDPRICINGSTRATEGY_H
#define OYO_HOTEL_BOOKING_LLD_STRATEGIES_STANDARDPRICINGSTRATEGY_H

#include <stdexcept>

#include "IPricingStrategy.h"

namespace oyo_hotel_lld {

using namespace std;

class StandardPricingStrategy : public IPricingStrategy {
public:
    double calculateTotal(double pricePerNight, int checkInDay, int checkOutDay) const override {
        if (checkOutDay <= checkInDay) {
            throw invalid_argument("checkOutDay must be after checkInDay");
        }
        // flat: har raat same daam
        return pricePerNight * static_cast<double>(checkOutDay - checkInDay);
    }
};

}  // namespace oyo_hotel_lld

#endif

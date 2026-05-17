#ifndef OYO_HOTEL_BOOKING_LLD_STRATEGIES_IPRICINGSTRATEGY_H
#define OYO_HOTEL_BOOKING_LLD_STRATEGIES_IPRICINGSTRATEGY_H

#include <bits/stdc++.h>

namespace oyo_hotel_lld {

using namespace std;

class IPricingStrategy {
public:
    virtual ~IPricingStrategy() = default;
    virtual double calculateTotal(double pricePerNight, int checkInDay, int checkOutDay) const = 0;
};

}  // namespace oyo_hotel_lld

#endif

#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_STRATEGIES_IPRICINGSTRATEGY_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_STRATEGIES_IPRICINGSTRATEGY_H

#include "../models/Seat.h"

namespace airline_mgmt {

class IPricingStrategy {
public:
    virtual ~IPricingStrategy() = default;
    virtual double calculateSeatFare(const Seat &seat) const = 0;
};

} // namespace airline_mgmt

#endif

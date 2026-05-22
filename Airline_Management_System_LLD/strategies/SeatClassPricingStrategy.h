#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_STRATEGIES_SEATCLASSPRICINGSTRATEGY_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_STRATEGIES_SEATCLASSPRICINGSTRATEGY_H

#include "../enums/SeatClass.h"
#include "IPricingStrategy.h"

namespace airline_mgmt {

class SeatClassPricingStrategy : public IPricingStrategy {
public:
    double calculateSeatFare(const Seat &seat) const override {
        switch (seat.seatClass) {
        case SeatClass::ECONOMY:
            return seat.basePrice;
        case SeatClass::PREMIUM_ECONOMY:
            return seat.basePrice * 1.4;
        case SeatClass::BUSINESS:
            return seat.basePrice * 2.5;
        }
        return seat.basePrice;
    }
};

} // namespace airline_mgmt

#endif

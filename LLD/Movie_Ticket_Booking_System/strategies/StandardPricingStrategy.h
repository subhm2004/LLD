#ifndef MOVIE_TICKET_BOOKING_SYSTEM_STRATEGIES_STANDARDPRICINGSTRATEGY_H
#define MOVIE_TICKET_BOOKING_SYSTEM_STRATEGIES_STANDARDPRICINGSTRATEGY_H

#include "PricingStrategy.h"

namespace movie_ticket_booking {

class StandardPricingStrategy : public PricingStrategy {
public:
    double getSeatPrice(SeatType seatType) const override {
        if (seatType == SeatType::REGULAR) return 180.0;
        if (seatType == SeatType::PREMIUM) return 260.0;
        return 350.0;
    }
};

}

#endif

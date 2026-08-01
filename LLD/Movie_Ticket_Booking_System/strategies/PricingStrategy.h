#ifndef MOVIE_TICKET_BOOKING_SYSTEM_STRATEGIES_PRICINGSTRATEGY_H
#define MOVIE_TICKET_BOOKING_SYSTEM_STRATEGIES_PRICINGSTRATEGY_H

#include "../enums/SeatType.h"

namespace movie_ticket_booking {

class PricingStrategy {
public:
    virtual ~PricingStrategy() = default;
    virtual double getSeatPrice(SeatType seatType) const = 0;
};

}

#endif

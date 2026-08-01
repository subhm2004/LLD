#ifndef MOVIE_TICKET_BOOKING_SYSTEM_FACTORIES_BOOKINGFACTORY_H
#define MOVIE_TICKET_BOOKING_SYSTEM_FACTORIES_BOOKINGFACTORY_H

#include "../models/Booking.h"

namespace movie_ticket_booking {

class BookingFactory {
public:
    Booking* createBooking(const std::string& bookingId,
                           const std::string& userId,
                           const std::string& showId,
                           const std::vector<std::string>& seatIds,
                           double amount) const {
        return new Booking(bookingId, userId, showId, seatIds, amount);
    }
};

}

#endif

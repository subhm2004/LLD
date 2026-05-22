#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_BOOKING_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_BOOKING_H

#include <string>
#include <vector>

#include "../enums/BookingStatus.h"

namespace airline_mgmt {

struct Booking {
    std::string bookingId;
    std::string passengerId;
    std::string flightId;
    std::vector<std::string> seatIds;
    BookingStatus status;
    double totalAmount;
};

} // namespace airline_mgmt

#endif

#ifndef IRCTC_LLD_MODELS_TICKETBOOKING_H
#define IRCTC_LLD_MODELS_TICKETBOOKING_H

#include <string>

#include "../enums/BookingStatus.h"

namespace irctc_lld {

struct TicketBooking {
    std::string bookingId;
    std::string userId;
    std::string trainNumber;
    std::string runDate;
    std::string seatId;
    std::string sourceStation;
    std::string destinationStation;
    int fromStationIdx{0};
    int toStationIdx{0};
    BookingStatus status{BookingStatus::CONFIRMED};
};

}  // namespace irctc_lld

#endif  // IRCTC_LLD_MODELS_TICKETBOOKING_H

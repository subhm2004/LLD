#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_SEAT_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_SEAT_H

#include <string>

#include "../enums/SeatClass.h"
#include "../enums/SeatStatus.h"

namespace airline_mgmt {

struct Seat {
    std::string seatId;
    std::string flightId;
    int row;
    int column;
    SeatClass seatClass;
    SeatStatus status;
    double basePrice;
};

} // namespace airline_mgmt

#endif

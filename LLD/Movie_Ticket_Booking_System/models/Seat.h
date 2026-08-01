#ifndef MOVIE_TICKET_BOOKING_SYSTEM_MODELS_SEAT_H
#define MOVIE_TICKET_BOOKING_SYSTEM_MODELS_SEAT_H

#include <string>
#include "../enums/SeatType.h"

namespace movie_ticket_booking {

class Seat {
public:
    Seat(std::string id, int row, int col, SeatType type)
        : seatId_(id), row_(row), col_(col), seatType_(type), booked_(false) {}

    const std::string& getSeatId() const { return seatId_; }
    SeatType getSeatType() const { return seatType_; }
    bool isBooked() const { return booked_; }
    void markBooked() { booked_ = true; }
    void markAvailable() { booked_ = false; }

private:
    std::string seatId_;
    int row_;
    int col_;
    SeatType seatType_;
    bool booked_;
};

}

#endif

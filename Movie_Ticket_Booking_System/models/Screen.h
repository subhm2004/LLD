#ifndef MOVIE_TICKET_BOOKING_SYSTEM_MODELS_SCREEN_H
#define MOVIE_TICKET_BOOKING_SYSTEM_MODELS_SCREEN_H

#include <string>
#include <vector>
#include "Seat.h"

namespace movie_ticket_booking {

class Screen {
public:
    explicit Screen(std::string id) : screenId_(id) {}

    const std::string& getId() const { return screenId_; }
    void addSeat(const Seat& seat) { seats_.push_back(seat); }
    std::vector<Seat>& getSeats() { return seats_; }
    const std::vector<Seat>& getSeats() const { return seats_; }

private:
    std::string screenId_;
    std::vector<Seat> seats_;
};

}

#endif

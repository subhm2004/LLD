#ifndef MOVIE_TICKET_BOOKING_SYSTEM_MODELS_SHOW_H
#define MOVIE_TICKET_BOOKING_SYSTEM_MODELS_SHOW_H

#include <string>
#include <vector>
#include "Movie.h"
#include "Seat.h"

namespace movie_ticket_booking {

class Show {
public:
    Show(std::string id, Movie movie, std::string startTime)
        : showId_(id), movie_(movie), startTime_(startTime) {}

    const std::string& getId() const { return showId_; }
    const Movie& getMovie() const { return movie_; }
    const std::string& getStartTime() const { return startTime_; }
    std::vector<Seat>& getSeats() { return seats_; }
    void setSeats(const std::vector<Seat>& seats) { seats_ = seats; }

private:
    std::string showId_;
    Movie movie_;
    std::string startTime_;
    std::vector<Seat> seats_;
};

}

#endif

#ifndef MOVIE_TICKET_BOOKING_SYSTEM_MODELS_MOVIE_H
#define MOVIE_TICKET_BOOKING_SYSTEM_MODELS_MOVIE_H

#include <string>

namespace movie_ticket_booking {

class Movie {
public:
    Movie(std::string id, std::string title, int durationMins)
        : movieId_(id), title_(title), durationMins_(durationMins) {}

    const std::string& getId() const { return movieId_; }
    const std::string& getTitle() const { return title_; }
    int getDurationMins() const { return durationMins_; }

private:
    std::string movieId_;
    std::string title_;
    int durationMins_;
};

}

#endif

#ifndef MOVIE_TICKET_BOOKING_SYSTEM_MODELS_THEATER_H
#define MOVIE_TICKET_BOOKING_SYSTEM_MODELS_THEATER_H

#include <string>
#include <vector>
#include "Screen.h"

namespace movie_ticket_booking {

class Theater {
public:
    Theater(std::string id, std::string name, std::string city)
        : theaterId_(id), name_(name), city_(city) {}

    const std::string& getId() const { return theaterId_; }
    const std::string& getCity() const { return city_; }
    void addScreen(const Screen& screen) { screens_.push_back(screen); }
    std::vector<Screen>& getScreens() { return screens_; }

private:
    std::string theaterId_;
    std::string name_;
    std::string city_;
    std::vector<Screen> screens_;
};

}

#endif

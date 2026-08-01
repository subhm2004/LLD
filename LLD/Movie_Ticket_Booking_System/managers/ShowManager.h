#ifndef MOVIE_TICKET_BOOKING_SYSTEM_MANAGERS_SHOWMANAGER_H
#define MOVIE_TICKET_BOOKING_SYSTEM_MANAGERS_SHOWMANAGER_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include "../models/Show.h"

namespace movie_ticket_booking {

class ShowManager {
public:
    void addShow(const Show& show) {
        auto it = shows_.find(show.getId());
        if (it != shows_.end()) {
            it->second = show;
        } else {
            shows_.emplace(show.getId(), show);
        }
    }

    Show& getShowById(const std::string& showId) {
        auto it = shows_.find(showId);
        if (it == shows_.end()) throw std::runtime_error("Show not found");
        return it->second;
    }

private:
    std::unordered_map<std::string, Show> shows_;
};

}

#endif

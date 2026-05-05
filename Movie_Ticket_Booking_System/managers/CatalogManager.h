#ifndef MOVIE_TICKET_BOOKING_SYSTEM_MANAGERS_CATALOGMANAGER_H
#define MOVIE_TICKET_BOOKING_SYSTEM_MANAGERS_CATALOGMANAGER_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include "../models/Movie.h"

namespace movie_ticket_booking {

class CatalogManager {
public:
    void addMovie(const Movie& movie) {
        auto it = movies_.find(movie.getId());
        if (it != movies_.end()) {
            it->second = movie;
        } else {
            movies_.emplace(movie.getId(), movie);
        }
    }

    Movie getMovieById(const std::string& movieId) const {
        auto it = movies_.find(movieId);
        if (it == movies_.end()) throw std::runtime_error("Movie not found");
        return it->second;
    }

private:
    std::unordered_map<std::string, Movie> movies_;
};

}

#endif

#ifndef MOVIE_TICKET_BOOKING_SYSTEM_MODELS_USER_H
#define MOVIE_TICKET_BOOKING_SYSTEM_MODELS_USER_H

#include <string>

namespace movie_ticket_booking {

class User {
public:
    User(std::string id, std::string name, std::string email)
        : userId_(id), name_(name), email_(email) {}

    const std::string& getId() const { return userId_; }
    const std::string& getName() const { return name_; }
    const std::string& getEmail() const { return email_; }

private:
    std::string userId_;
    std::string name_;
    std::string email_;
};

}

#endif

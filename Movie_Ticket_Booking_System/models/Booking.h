#ifndef MOVIE_TICKET_BOOKING_SYSTEM_MODELS_BOOKING_H
#define MOVIE_TICKET_BOOKING_SYSTEM_MODELS_BOOKING_H

#include <string>
#include <vector>
#include "../enums/BookingStatus.h"

namespace movie_ticket_booking {

class Booking {
public:
    Booking(std::string id, std::string userId, std::string showId, std::vector<std::string> seatIds, double amount)
        : bookingId_(id),
          userId_(userId),
          showId_(showId),
          seatIds_(seatIds),
          totalAmount_(amount),
          status_(BookingStatus::CREATED) {}

    const std::string& getId() const { return bookingId_; }
    const std::string& getShowId() const { return showId_; }
    const std::vector<std::string>& getSeatIds() const { return seatIds_; }
    double getTotalAmount() const { return totalAmount_; }
    BookingStatus getStatus() const { return status_; }
    void setStatus(BookingStatus status) { status_ = status; }

private:
    std::string bookingId_;
    std::string userId_;
    std::string showId_;
    std::vector<std::string> seatIds_;
    double totalAmount_;
    BookingStatus status_;
};

}

#endif

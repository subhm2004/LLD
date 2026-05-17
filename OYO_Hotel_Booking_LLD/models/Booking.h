#ifndef OYO_HOTEL_BOOKING_LLD_MODELS_BOOKING_H
#define OYO_HOTEL_BOOKING_LLD_MODELS_BOOKING_H

#include <bits/stdc++.h>

#include "../enums/BookingStatus.h"

namespace oyo_hotel_lld {

using namespace std;

class Booking {
public:
    Booking(string bookingId,
            string guestId,
            string hotelId,
            string roomId,
            int checkInDay,
            int checkOutDay,
            double totalAmount)
        : bookingId_(std::move(bookingId)),
          guestId_(std::move(guestId)),
          hotelId_(std::move(hotelId)),
          roomId_(std::move(roomId)),
          checkInDay_(checkInDay),
          checkOutDay_(checkOutDay),
          totalAmount_(totalAmount),
          status_(BookingStatus::PENDING) {}

    const string &getBookingId() const { return bookingId_; }
    const string &getGuestId() const { return guestId_; }
    const string &getHotelId() const { return hotelId_; }
    const string &getRoomId() const { return roomId_; }
    int getCheckInDay() const { return checkInDay_; }
    int getCheckOutDay() const { return checkOutDay_; }
    double getTotalAmount() const { return totalAmount_; }
    BookingStatus getStatus() const { return status_; }

    void setStatus(BookingStatus status) { status_ = status; }

    int getNights() const { return checkOutDay_ - checkInDay_; }

private:
    string bookingId_;
    string guestId_;
    string hotelId_;
    string roomId_;
    int checkInDay_;
    int checkOutDay_;
    double totalAmount_;
    BookingStatus status_;
};

}  // namespace oyo_hotel_lld

#endif

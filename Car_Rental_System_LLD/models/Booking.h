#ifndef CAR_RENTAL_SYSTEM_LLD_MODELS_BOOKING_H
#define CAR_RENTAL_SYSTEM_LLD_MODELS_BOOKING_H

#include <bits/stdc++.h>

#include "../enums/BookingStatus.h"

using namespace std;

namespace car_rental_lld {

class Booking {
public:
    Booking(string bookingId, string customerId, string vehicleId, int startDay, int endDay, double totalAmount)
        : bookingId_(std::move(bookingId)),
          customerId_(std::move(customerId)),
          vehicleId_(std::move(vehicleId)),
          startDay_(startDay),
          endDay_(endDay),
          totalAmount_(totalAmount),
          status_(BookingStatus::CREATED) {}

    const string &getId() const { return bookingId_; }
    const string &getCustomerId() const { return customerId_; }
    const string &getVehicleId() const { return vehicleId_; }
    int getStartDay() const { return startDay_; }
    int getEndDay() const { return endDay_; }
    double getTotalAmount() const { return totalAmount_; }
    BookingStatus getStatus() const { return status_; }

    void setStatus(BookingStatus status) { status_ = status; }

private:
    string bookingId_;
    string customerId_;
    string vehicleId_;
    int startDay_;
    int endDay_;
    double totalAmount_;
    BookingStatus status_;
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_MODELS_BOOKING_H

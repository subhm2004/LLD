#ifndef OYO_HOTEL_BOOKING_LLD_SERVICES_NOTIFICATIONSERVICE_H
#define OYO_HOTEL_BOOKING_LLD_SERVICES_NOTIFICATIONSERVICE_H

#include <bits/stdc++.h>

namespace oyo_hotel_lld {

using namespace std;

class NotificationService {
public:
    void sendBookingConfirmed(const string &guestEmail, const string &bookingId, double amount) const {
        cout << "[Email:" << guestEmail << "] Booking " << bookingId << " confirmed. Amount: Rs " << amount
             << "\n";
    }

    void sendBookingCancelled(const string &guestEmail, const string &bookingId) const {
        cout << "[Email:" << guestEmail << "] Booking " << bookingId << " cancelled.\n";
    }

    void sendCheckInReminder(const string &guestPhone, const string &hotelName, int checkInDay) const {
        cout << "[SMS:" << guestPhone << "] Check-in tomorrow at " << hotelName << " (day " << checkInDay
             << ")\n";
    }
};

}  // namespace oyo_hotel_lld

#endif

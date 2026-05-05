#ifndef MOVIE_TICKET_BOOKING_SYSTEM_EXTERNAL_NOTIFICATIONSERVICE_H
#define MOVIE_TICKET_BOOKING_SYSTEM_EXTERNAL_NOTIFICATIONSERVICE_H

#include <iostream>
#include <string>

namespace movie_ticket_booking {

class NotificationService {
public:
    void sendBookingConfirmation(const std::string& email, const std::string& bookingId) const {
        std::cout << "Notification sent to " << email << " for booking " << bookingId << "\n";
    }
};

}

#endif

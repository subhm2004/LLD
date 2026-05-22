#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_SERVICES_NOTIFICATIONSERVICE_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_SERVICES_NOTIFICATIONSERVICE_H

#include <iostream>
#include <string>

namespace airline_mgmt {

class NotificationService {
public:
    void sendBookingConfirmed(const std::string &email, const std::string &bookingId, double amount) const {
        std::cout << "[NOTIFY] Booking confirmed -> " << email << " | " << bookingId << " | Rs " << amount << "\n";
    }

    void sendRefund(const std::string &email, const std::string &bookingId, double amount) const {
        std::cout << "[NOTIFY] Refund processed -> " << email << " | " << bookingId << " | Rs " << amount << "\n";
    }

    void sendFlightChange(const std::string &email, const std::string &flightId) const {
        std::cout << "[NOTIFY] Flight schedule change -> " << email << " | flight " << flightId << "\n";
    }
};

} // namespace airline_mgmt

#endif

#ifndef AMAZON_LOCKER_SERVICE_LLD_SERVICES_NOTIFICATIONSERVICE_H
#define AMAZON_LOCKER_SERVICE_LLD_SERVICES_NOTIFICATIONSERVICE_H

#include <iostream>
#include <string>

namespace amazon_locker_lld {

class NotificationService {
public:
    void notifyPickupCode(const std::string& customerId, const std::string& stationId,
                          const std::string& compartmentId, const std::string& code, int validHours) {
        std::cout << "[Notify] customer=" << customerId << " | station=" << stationId
                  << " | compartment=" << compartmentId << " | code=" << code
                  << " | valid " << validHours << "h\n";
    }

    void notifyPickupSuccess(const std::string& customerId, const std::string& packageId) {
        std::cout << "[Notify] Pickup confirmed for customer=" << customerId
                  << " package=" << packageId << "\n";
    }
};

}  // namespace amazon_locker_lld

#endif  // AMAZON_LOCKER_SERVICE_LLD_SERVICES_NOTIFICATIONSERVICE_H

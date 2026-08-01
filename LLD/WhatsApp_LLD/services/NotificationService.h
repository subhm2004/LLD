#ifndef NOTIFICATION_SERVICE_H
#define NOTIFICATION_SERVICE_H

#include <iostream>
#include <string>

namespace whatsapp_lld {

class NotificationService {
public:
    void notifyUser(const std::string& userId, const std::string& message) const {
        std::cout << "[Notification] user=" << userId << " message=" << message << std::endl;
    }
};

}  // namespace whatsapp_lld

#endif

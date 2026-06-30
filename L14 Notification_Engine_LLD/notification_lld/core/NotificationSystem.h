// core/NotificationSystem.h — System ka core orchestrator/facade: message banao,
// rules check karo, engine ke through channels par bhejo, observers ko notify karo.
#ifndef NOTIFICATION_SYSTEM_H
#define NOTIFICATION_SYSTEM_H

#include <memory>
#include <string>
#include "../enums/ChannelType.h"
#include "../models/NotificationMessage.h"
#include "../models/Recipient.h"
#include "../services/NotificationEngine.h"
#include "../services/NotificationService.h"

namespace notification_lld {

class NotificationSystem {
private:
    NotificationService notificationService_;

public:
    void registerRecipient(const std::string& id, const std::string& name, const std::string& address,
                           ChannelType preferredChannel) {
        notificationService_.registerRecipient(Recipient(id, name, address, preferredChannel));
    }

    void setupDefaultEngine() {
        NotificationObservable* observable = NotificationHub::getInstance().getObservable();
        static LoggerObserver logger(observable);
        static NotificationEngine engine(observable);
        static bool initialized = false;
        if (!initialized) {
            engine.addNotificationStrategy(std::make_unique<EmailStrategy>("alerts@system.com"));
            engine.addNotificationStrategy(std::make_unique<SMSStrategy>("+91-9000000000"));
            engine.addNotificationStrategy(std::make_unique<PopUpStrategy>());
            initialized = true;
        }
    }

    void sendNotification(const std::string& recipientId, const std::string& messageId,
                          const std::string& title, const std::string& body,
                          NotificationPriority priority) {
        NotificationMessage message(messageId, title, body, priority);
        notificationService_.send(recipientId, message);
    }
};

}  // namespace notification_lld

#endif

// services/NotificationService.h — SINGLETON entry point + Observable subject.
// Naya notification set hote hi observers (Logger, NotificationEngine) ko notify
// karta hai. Poore system ka central coordinator.
#ifndef NOTIFICATION_SERVICE_H
#define NOTIFICATION_SERVICE_H

#include <memory>
#include <string>
#include <unordered_map>
#include "../models/NotificationMessage.h"
#include "../models/Recipient.h"
#include "NotificationEngine.h"

namespace notification_lld {

class NotificationService {
private:
    std::unordered_map<std::string, Recipient> recipients_;

public:
    void registerRecipient(const Recipient& recipient) {
        recipients_.emplace(recipient.getId(), recipient);
    }

    bool hasRecipient(const std::string& recipientId) const {
        return recipients_.find(recipientId) != recipients_.end();
    }

    void send(const std::string& recipientId, const NotificationMessage& message) {
        auto it = recipients_.find(recipientId);
        if (it == recipients_.end()) {
            return;
        }

        std::shared_ptr<INotification> notification =
            std::make_shared<SimpleNotification>(message.getTitle() + ": " + message.getBody());
        notification = std::make_shared<TimestampDecorator>(notification);
        notification = std::make_shared<SignatureDecorator>(notification, "NotificationService");
        NotificationHub::getInstance().sendNotification(notification);
    }
};

}  // namespace notification_lld

#endif

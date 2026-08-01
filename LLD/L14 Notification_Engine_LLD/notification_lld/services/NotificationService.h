// ============================================================================
//  services/NotificationService.h — recipients ka registry + DECORATOR assembly
// ----------------------------------------------------------------------------
//  Do kaam:
//    1. Recipients store karta hai (id -> Recipient map) — kisko bhejna hai
//    2. send() me DECORATOR CHAIN banata hai aur Hub ko deta hai:
//
//    SimpleNotification("title: body")           <- base
//        wrapped in TimestampDecorator            <- +timestamp
//            wrapped in SignatureDecorator         <- +signature
//        -> NotificationHub.sendNotification()     <- observers ko trigger
//
//  Yahi wo jagah hai jahan DECORATOR pattern actually USE hota hai —
//  base notification ko step-by-step "sajaya" jaata hai. Har make_shared
//  ek naya layer add karta hai jo pichle ko wrap karta hai.
//
//  ⚠️ NOTE: agar recipient id na mile to send() chupchap return kar deta
//  hai (koi error/log nahi) — production me ye log/exception hona chahiye.
// ============================================================================
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

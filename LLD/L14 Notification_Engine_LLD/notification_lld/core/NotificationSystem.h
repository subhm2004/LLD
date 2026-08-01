// ============================================================================
//  core/NotificationSystem.h — FACADE: system ka simple entry-point
// ----------------------------------------------------------------------------
//  Client ko andar ke Hub/Observable/Engine/Strategies/Decorators ka jhamela
//  nahi dekhna padta — bas 3 aasaan methods:
//    registerRecipient() -> kaun, kaunse channel pe notification chahta hai
//    setupDefaultEngine()-> Logger + Engine ko observable pe attach karo, aur
//                           3 channel strategies (Email/SMS/Popup) add karo
//    sendNotification()  -> message banao aur bhejo (baaki sab automatic)
//
//  ⭐ setupDefaultEngine() ka `static` + `initialized` flag trick: Logger/
//  Engine sirf EK baar setup hon (dobara call pe strategies double na ho
//  jayein). static local objects program-lifetime tak zinda rehte hain —
//  isliye observable se attached rehte hain jab tak main chalta hai.
// ============================================================================
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

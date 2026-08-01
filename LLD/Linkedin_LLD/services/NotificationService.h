// ============================================================================
//  services/NotificationService.h  —  OBSERVER PATTERN (Requirement R10)
// ----------------------------------------------------------------------------
//  R10 (Notifications): "The system will notify users about new messages,
//  connection requests, and interactions with their content, keeping them
//  informed and engaged."
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ OBSERVER PATTERN — "ek event hua, sab interested ko khabar kar do"    │
//  │                                                                          │
//  │  SUBJECT   : NotificationService — events aate hain, ye sabko batata hai │
//  │  OBSERVER  : INotificationObserver — jo bhi "sunna" chahe (interface)    │
//  │  CONCRETE  : ConsoleNotificationObserver — console pe print kar deta hai │
//  │                                                                          │
//  │  Fayda: kal ko Email/SMS/Push bhejni ho? Bas naya observer bana ke       │
//  │  register kar do — NotificationService ka ek line bhi nahi badlega.      │
//  │  (Yahi Open/Closed Principle hai.)                                       │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Ye service DO kaam karti hai:
//     1. STORE: har recipient ke inbox me Notification daal deti hai (taaki
//        user baad me apni notifications dekh sake — "3 unread")
//     2. BROADCAST: sabhi registered observers ko turant bata deti hai
//        (real-time "ping" — jaise phone pe notification aana)
//
//  📌 LinkedinSystem (facade) teen jagah is service ko bulata hai — bilkul R10
//     ke teen kaaran: connection request, naya message, post pe interaction.
// ============================================================================
#ifndef LINKEDIN_LLD_SERVICES_NOTIFICATIONSERVICE_H
#define LINKEDIN_LLD_SERVICES_NOTIFICATIONSERVICE_H

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../enums/NotificationType.h"
#include "../models/Notification.h"

namespace linkedin_lld {

// ============================================================================
//  OBSERVER INTERFACE — jo bhi notifications "sunna" chahe, ye implement kare
// ============================================================================
class INotificationObserver {
public:
    virtual ~INotificationObserver() = default;
    // Jab bhi koi notification bane, ye method har observer pe call hota hai.
    virtual void onNotification(const Notification &notification) = 0;
};

// ============================================================================
//  CONCRETE OBSERVER — console pe print kar deta hai (demo ke liye)
// ============================================================================
class ConsoleNotificationObserver : public INotificationObserver {
public:
    void onNotification(const Notification &notification) override {
        std::cout << "    🔔 [" << notificationTypeToString(notification.getType()) << " -> "
                  << notification.getRecipientId() << "] " << notification.getText() << "\n";
    }
};

// ============================================================================
//  SUBJECT — NotificationService
// ============================================================================
class NotificationService {
public:
    // Observer register karo (Subject ki list me add). shared_ptr taaki lifetime
    // safe rahe — jab tak service ke paas hai, observer zinda rahega.
    void registerObserver(const std::shared_ptr<INotificationObserver> &observer) {
        if (observer) {
            observers_.push_back(observer);
        }
    }

    // ---- Ek notification bhejo: STORE + BROADCAST (R10) ---------------------
    void notify(const std::string &recipientId, NotificationType type, const std::string &text) {
        Notification notification(recipientId, type, text);

        // 1. STORE — recipient ke inbox me daalo (baad me dekhne ke liye)
        inbox_[recipientId].push_back(notification);

        // 2. BROADCAST — sab observers ko turant batao (real-time ping)
        for (const auto &observer : observers_) {
            observer->onNotification(notification);
        }
    }

    // ---- Kisi user ki saari notifications (inbox) ---------------------------
    const std::vector<Notification> &getInbox(const std::string &userId) const {
        static const std::vector<Notification> empty; // user ka inbox khaali/nahi hai
        auto it = inbox_.find(userId);
        return it == inbox_.end() ? empty : it->second;
    }

    // ---- Kitni unread notifications hain ------------------------------------
    int getUnreadCount(const std::string &userId) const {
        auto it = inbox_.find(userId);
        if (it == inbox_.end()) {
            return 0;
        }
        int count = 0;
        for (const Notification &n : it->second) {
            if (!n.isRead()) {
                ++count;
            }
        }
        return count;
    }

private:
    std::vector<std::shared_ptr<INotificationObserver>> observers_;      // sunne wale (broadcast)
    std::unordered_map<std::string, std::vector<Notification>> inbox_;   // recipientId -> notifications (store)
};

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_SERVICES_NOTIFICATIONSERVICE_H

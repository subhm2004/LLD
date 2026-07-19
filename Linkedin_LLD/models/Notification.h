// ============================================================================
//  models/Notification.h  —  User ko dikhne wali ek notification (Requirement R10)
// ----------------------------------------------------------------------------
//  Requirement R10 (Notifications): "The system will notify users about new
//  messages, connection requests, and interactions with their content."
//
//  Ek notification: kise (recipientId), kis wajah se (NotificationType), kya
//  message (text), kab (createdAt), aur padhi gayi ya nahi (read_).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Notification kaun BANATA/BHEJTA hai? -> NotificationService (Observer)│
//  │                                                                          │
//  │  Jab bhi koi event hota hai (message aaya, request aayi, post pe react),  │
//  │  NotificationService ek Notification bana ke recipient ke inbox me daal   │
//  │  deti hai. Ye class sirf us notification ka DATA hai.                     │
//  │                                                                          │
//  │  `read_` flag: shuru me false (unread). User dekh le to true. Isse       │
//  │  "aapke paas 3 unread notifications hain" wala count dikhaya ja sakta.    │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef LINKEDIN_LLD_MODELS_NOTIFICATION_H
#define LINKEDIN_LLD_MODELS_NOTIFICATION_H

#include <ctime>
#include <string>
#include <utility>

#include "../enums/NotificationType.h"

namespace linkedin_lld {

class Notification {
public:
    Notification(std::string recipientId, NotificationType type, std::string text)
        : recipientId_(std::move(recipientId)),
          type_(type),
          text_(std::move(text)),
          createdAt_(std::time(nullptr)) {}

    const std::string &getRecipientId() const { return recipientId_; }
    NotificationType getType() const { return type_; }
    const std::string &getText() const { return text_; }
    long long getCreatedAt() const { return createdAt_; }

    bool isRead() const { return read_; }
    void markRead() { read_ = true; }

private:
    std::string recipientId_;   // kise mili
    NotificationType type_;     // kis wajah se (R10 ke teen kaaran)
    std::string text_;          // "Ananya ne aapke post pe react kiya"
    long long createdAt_;
    bool read_ = false;         // shuru me unread
};

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_MODELS_NOTIFICATION_H

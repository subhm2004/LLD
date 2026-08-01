// ============================================================================
//  enums/NotificationType.h  —  Notification kis wajah se aayi (Requirement R10)
// ----------------------------------------------------------------------------
//  Requirement R10 (Notifications): "The system will notify users about new
//  MESSAGES, CONNECTION REQUESTS, and INTERACTIONS with their content."
//
//  Image me teen wajahein saaf likhi hain, to teen types:
//
//     CONNECTION_REQUEST -> kisi ne aapko connect hone ki request bheji /
//                           accept ki
//     MESSAGE            -> koi naya message aaya
//     POST_INTERACTION   -> aapke post pe kisi ne react/comment/share kiya
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Ye enum "kyun aayi" batata hai — NotificationService "kaise bheji"    │
//  │     dekhta hai (Observer pattern). Dono alag zimmedari.                   │
//  │                                                                          │
//  │  Type ka fayda: user chahe to filter kar sake ("sirf messages dikhao"),  │
//  │  ya har type ka alag icon/priority de sake. Ek generic string message ke │
//  │  bajaye TYPED notification zyada kaam ki hai.                            │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef LINKEDIN_LLD_ENUMS_NOTIFICATIONTYPE_H
#define LINKEDIN_LLD_ENUMS_NOTIFICATIONTYPE_H

#include <string>

namespace linkedin_lld {

enum class NotificationType { CONNECTION_REQUEST, MESSAGE, POST_INTERACTION };

// Notification type ko padhne-layak string me — inbox printing ke liye.
inline std::string notificationTypeToString(NotificationType type) {
    switch (type) {
        case NotificationType::CONNECTION_REQUEST: return "CONNECTION_REQUEST";
        case NotificationType::MESSAGE:            return "MESSAGE";
        case NotificationType::POST_INTERACTION:   return "POST_INTERACTION";
    }
    return "UNKNOWN";
}

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_ENUMS_NOTIFICATIONTYPE_H

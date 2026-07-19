// ============================================================================
//  enums/NotificationPriority.h — Priority: LOW / MEDIUM / HIGH
// ----------------------------------------------------------------------------
//  Message ki urgency. Abhi mostly data ke roop me store hota hai; Rules
//  engine (ya future logic) isse use karke decide kar sakta hai kaunsa
//  message pehle/kaise bheje (jaise HIGH turant, LOW batch me). enum class
//  (scoped, type-safe).
// ============================================================================
#ifndef NOTIFICATION_PRIORITY_H
#define NOTIFICATION_PRIORITY_H

namespace notification_lld {

enum class NotificationPriority {
    LOW,
    MEDIUM,
    HIGH
};

}  // namespace notification_lld

#endif

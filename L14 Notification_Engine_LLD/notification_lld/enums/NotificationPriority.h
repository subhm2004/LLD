// enums/NotificationPriority.h — Notification ki priority: LOW / MEDIUM / HIGH.
// Rules engine isse use karke decide kar sakta hai konsa message kaise/kab bheje.
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

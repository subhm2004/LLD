// rules/NotificationRules.h — Business rules: kaunsa message bheja jaaye ya
// skip ho (jaise priority/channel filter, rate-limit). Delivery se pehle check.
#ifndef NOTIFICATION_RULES_H
#define NOTIFICATION_RULES_H

namespace notification_lld {

struct NotificationRules {
    static constexpr int kMaxRetries = 3;
    static constexpr int kBatchSize = 100;
};

}  // namespace notification_lld

#endif

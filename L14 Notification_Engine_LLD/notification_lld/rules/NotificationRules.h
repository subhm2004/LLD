#ifndef NOTIFICATION_RULES_H
#define NOTIFICATION_RULES_H

namespace notification_lld {

struct NotificationRules {
    static constexpr int kMaxRetries = 3;
    static constexpr int kBatchSize = 100;
};

}  // namespace notification_lld

#endif

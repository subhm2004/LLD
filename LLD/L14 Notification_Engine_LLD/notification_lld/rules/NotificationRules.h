// ============================================================================
//  rules/NotificationRules.h — Business rules ke constants (config placeholder)
// ----------------------------------------------------------------------------
//  Abhi sirf 2 config constants: kMaxRetries (fail hone pe kitni baar retry)
//  aur kBatchSize (ek baar me kitne messages batch me). Ye future extension
//  ka placeholder hai — yahan rate-limiting, priority-filtering, retry logic
//  add ho sakta hai. Abhi actual delivery flow me use nahi hota, par LLD me
//  aisi rules-class rakhna good practice hai (separation of concerns).
// ============================================================================
#ifndef NOTIFICATION_RULES_H
#define NOTIFICATION_RULES_H

namespace notification_lld {

struct NotificationRules {
    static constexpr int kMaxRetries = 3;
    static constexpr int kBatchSize = 100;
};

}  // namespace notification_lld

#endif

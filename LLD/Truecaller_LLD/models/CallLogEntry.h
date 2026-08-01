// ============================================================================
//  models/CallLogEntry.h  —  Call history ka ek record
// ----------------------------------------------------------------------------
//  Kisne, kisko/kisse, kis kism ki, kab — bas itna.
//
//  ⭐ Poori tarah IMMUTABLE hai (ek bhi setter nahi). Call ho chuki — ab uska
//     record badalna kabhi sahi nahi. History historical honi chahiye.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `remotePhone_` naam kyun (callerPhone/calleePhone nahi)?               │
//  │                                                                          │
//  │  Kyunki ek hi field DONO kaam karti hai — `type_` batata hai kaun kis     │
//  │  taraf tha:                                                              │
//  │      INCOMING/MISSED -> remotePhone ne CALL KI                            │
//  │      OUTGOING        -> remotePhone ko CALL KI GAYI                       │
//  │  Do alag fields (caller + callee) rakhte to ek hamesha khaali rehti aur   │
//  │  har jagah `if (type == INCOMING)` likhna padta.                         │
//  │  📌 "Remote" = doosri taraf wala — ye telephony ki standard term hai.    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Timestamp `long long` epoch SECONDS me hai — `std::string` date nahi.
//     Fayde: compare/sort seedha number comparison hai (sasta), timezone ka
//     jhamela nahi, aur storage compact. Formatting sirf DISPLAY ke waqt hoti
//     hai — data hamesha raw rakho, format bahar karo.
//
//  ⚠ SECONDS resolution ka ek asli side-effect: ek hi second me hui saari calls
//     ka timestamp BILKUL SAME hota hai. Isliye "recent first" sort ke liye
//     sirf timestamp kaafi NAHI — tie-break chahiye. Ye ek asli bug tha,
//     `CallLogService.h` me poora note aur fix hai. Milliseconds use karne se
//     bhi ye poori tarah nahi jaata (tez system me same ms bhi possible hai).
// ============================================================================
#ifndef TRUECALLER_LLD_MODELS_CALLLOGENTRY_H
#define TRUECALLER_LLD_MODELS_CALLLOGENTRY_H

#include <string>
#include <utility>

#include "../enums/CallType.h"
#include "../utils/PhoneUtils.h"

namespace truecaller_lld {

class CallLogEntry {
public:
    CallLogEntry() = default;

    CallLogEntry(std::string callId, std::string userId, std::string remotePhone, CallType type,
                 long long timestampEpochSec)
        : callId_(std::move(callId)),
          userId_(std::move(userId)),                        // kiski history me
          remotePhone_(normalizePhone(std::move(remotePhone))), // ⭐ doosri taraf wala
          type_(type),                                       // ⭐ direction batata hai
          timestampEpochSec_(timestampEpochSec) {}

    const std::string& getCallId() const { return callId_; }
    const std::string& getUserId() const { return userId_; }
    const std::string& getRemotePhone() const { return remotePhone_; }
    CallType getType() const { return type_; }
    long long getTimestampEpochSec() const { return timestampEpochSec_; }
    // ⚠ koi setter nahi — call ho chuki, record immutable

private:
    std::string callId_;
    std::string userId_;
    std::string remotePhone_;
    CallType type_{CallType::INCOMING};
    long long timestampEpochSec_{0}; // ⚠ seconds -> same-second ties (upar note)
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_MODELS_CALLLOGENTRY_H

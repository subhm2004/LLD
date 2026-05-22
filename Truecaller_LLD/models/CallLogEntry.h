#ifndef TRUECALLER_LLD_MODELS_CALLLOGENTRY_H
#define TRUECALLER_LLD_MODELS_CALLLOGENTRY_H

#include <chrono>
#include <string>

#include "../enums/CallType.h"
#include "../utils/PhoneUtils.h"

namespace truecaller_lld {

class CallLogEntry {
public:
    CallLogEntry() = default;

    CallLogEntry(std::string callId, std::string userId, std::string remotePhone, CallType type,
                 long long timestampEpochSec)
        : callId_(std::move(callId)),
          userId_(std::move(userId)),
          remotePhone_(normalizePhone(std::move(remotePhone))),
          type_(type),
          timestampEpochSec_(timestampEpochSec) {}

    const std::string& getCallId() const { return callId_; }
    const std::string& getUserId() const { return userId_; }
    const std::string& getRemotePhone() const { return remotePhone_; }
    CallType getType() const { return type_; }
    long long getTimestampEpochSec() const { return timestampEpochSec_; }

private:
    std::string callId_;
    std::string userId_;
    std::string remotePhone_;
    CallType type_{CallType::INCOMING};
    long long timestampEpochSec_{0};
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_MODELS_CALLLOGENTRY_H

#ifndef TRUECALLER_LLD_SERVICES_CALLLOGSERVICE_H
#define TRUECALLER_LLD_SERVICES_CALLLOGSERVICE_H

#include <algorithm>
#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>

#include "../enums/CallType.h"
#include "../models/CallLogEntry.h"

namespace truecaller_lld {

class CallLogService {
public:
    std::string logCall(const std::string& userId, const std::string& remotePhone, CallType type) {
        const std::string callId = "CALL_" + std::to_string(++callCounter_);
        const long long now = nowEpoch();
        callLogs_[userId].emplace_back(callId, userId, remotePhone, type, now);
        return callId;
    }

    std::vector<CallLogEntry> getRecentCalls(const std::string& userId, size_t limit = 20) const {
        auto it = callLogs_.find(userId);
        if (it == callLogs_.end()) {
            return {};
        }
        std::vector<CallLogEntry> copy = it->second;
        std::sort(copy.begin(), copy.end(),
                  [](const CallLogEntry& a, const CallLogEntry& b) {
                      return a.getTimestampEpochSec() > b.getTimestampEpochSec();
                  });
        if (copy.size() > limit) {
            copy.resize(limit);
        }
        return copy;
    }

private:
    std::unordered_map<std::string, std::vector<CallLogEntry>> callLogs_;
    int callCounter_{0};

    static long long nowEpoch() {
        return std::chrono::duration_cast<std::chrono::seconds>(
                   std::chrono::system_clock::now().time_since_epoch())
            .count();
    }
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_SERVICES_CALLLOGSERVICE_H

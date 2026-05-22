#ifndef TRUECALLER_LLD_SERVICES_SPAMREPORTSERVICE_H
#define TRUECALLER_LLD_SERVICES_SPAMREPORTSERVICE_H

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "../enums/CallerTag.h"
#include "../enums/ReportReason.h"
#include "../models/PhoneProfile.h"
#include "../strategies/ISpamScoringStrategy.h"
#include "../utils/PhoneUtils.h"

namespace truecaller_lld {

class SpamReportService {
public:
    explicit SpamReportService(std::shared_ptr<ISpamScoringStrategy> strategy)
        : strategy_(std::move(strategy)) {}

    void reportSpam(const std::string& reporterUserId, const std::string& phone,
                    ReportReason reason,
                    std::unordered_map<std::string, PhoneProfile>& directory) {
        if (reporterUserId.empty()) {
            throw std::invalid_argument("reporter user id required");
        }
        const std::string normalized = normalizePhone(phone);

        auto it = directory.find(normalized);
        if (it == directory.end()) {
            directory.emplace(normalized, PhoneProfile(normalized, "Reported Number", "", CallerTag::UNKNOWN));
            it = directory.find(normalized);
        }

        const int delta = strategy_->scoreDeltaFor(reason);
        it->second.addSpamReport(delta);

        if (reason == ReportReason::TELEMARKETER && it->second.getSpamScore() < 70) {
            it->second.setPrimaryTag(CallerTag::TELEMARKETER);
        }
    }

private:
    std::shared_ptr<ISpamScoringStrategy> strategy_;
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_SERVICES_SPAMREPORTSERVICE_H

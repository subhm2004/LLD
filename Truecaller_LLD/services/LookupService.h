#ifndef TRUECALLER_LLD_SERVICES_LOOKUPSERVICE_H
#define TRUECALLER_LLD_SERVICES_LOOKUPSERVICE_H

#include <string>
#include <unordered_map>

#include "../models/CallerLookupResult.h"
#include "../models/PhoneProfile.h"
#include "../utils/PhoneUtils.h"
#include "BlockService.h"

namespace truecaller_lld {

class LookupService {
public:
    CallerLookupResult lookup(const std::string& forUserId, const std::string& callerPhone,
                              const std::unordered_map<std::string, PhoneProfile>& directory,
                              const BlockService& blockService) const {
        const std::string phone = normalizePhone(callerPhone);
        CallerLookupResult result;
        result.phone = phone;
        result.isBlocked = blockService.isBlocked(forUserId, phone);

        auto it = directory.find(phone);
        if (it == directory.end()) {
            result.displayName = "Unknown";
            result.tag = CallerTag::UNKNOWN;
            return result;
        }

        const PhoneProfile& profile = it->second;
        result.foundInDirectory = true;
        result.displayName = profile.getDisplayName().empty() ? "Unknown" : profile.getDisplayName();
        result.tag = profile.getPrimaryTag();
        result.spamScore = profile.getSpamScore();
        result.isSpam = profile.isSpam();
        return result;
    }
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_SERVICES_LOOKUPSERVICE_H

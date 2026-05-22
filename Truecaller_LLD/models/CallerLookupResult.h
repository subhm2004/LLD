#ifndef TRUECALLER_LLD_MODELS_CALLERLOOKUPRESULT_H
#define TRUECALLER_LLD_MODELS_CALLERLOOKUPRESULT_H

#include <string>

#include "../enums/CallerTag.h"

namespace truecaller_lld {

struct CallerLookupResult {
    std::string phone;
    std::string displayName;
    CallerTag tag{CallerTag::UNKNOWN};
    int spamScore{0};
    bool isSpam{false};
    bool isBlocked{false};
    bool foundInDirectory{false};
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_MODELS_CALLERLOOKUPRESULT_H

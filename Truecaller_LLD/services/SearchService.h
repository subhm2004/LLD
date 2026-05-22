#ifndef TRUECALLER_LLD_SERVICES_SEARCHSERVICE_H
#define TRUECALLER_LLD_SERVICES_SEARCHSERVICE_H

#include <string>
#include <unordered_map>
#include <vector>

#include "../models/PhoneProfile.h"
#include "../utils/PhoneUtils.h"

namespace truecaller_lld {

class SearchService {
public:
    std::vector<PhoneProfile> searchByNamePrefix(
        const std::string& prefix,
        const std::unordered_map<std::string, PhoneProfile>& directory,
        size_t limit = 10) const {
        const std::string key = toLower(prefix);
        std::vector<PhoneProfile> matches;

        for (const auto& entry : directory) {
            const PhoneProfile& profile = entry.second;
            if (profile.getDisplayName().empty()) {
                continue;
            }
            const std::string name = toLower(profile.getDisplayName());
            if (name.rfind(key, 0) == 0) {
                matches.push_back(profile);
                if (matches.size() >= limit) {
                    break;
                }
            }
        }
        return matches;
    }

    std::vector<PhoneProfile> searchByPhone(
        const std::string& phone,
        const std::unordered_map<std::string, PhoneProfile>& directory) const {
        const std::string normalized = normalizePhone(phone);
        auto it = directory.find(normalized);
        if (it == directory.end()) {
            return {};
        }
        return {it->second};
    }
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_SERVICES_SEARCHSERVICE_H

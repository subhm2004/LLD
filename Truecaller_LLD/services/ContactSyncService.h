#ifndef TRUECALLER_LLD_SERVICES_CONTACTSYNCSERVICE_H
#define TRUECALLER_LLD_SERVICES_CONTACTSYNCSERVICE_H

#include <string>
#include <unordered_map>
#include <vector>

#include "../models/ContactEntry.h"
#include "../models/PhoneProfile.h"

namespace truecaller_lld {

class ContactSyncService {
public:
    std::vector<ContactEntry> syncContacts(
        const std::vector<ContactEntry>& uploaded,
        const std::unordered_map<std::string, PhoneProfile>& directory) const {
        std::vector<ContactEntry> enriched;
        enriched.reserve(uploaded.size());

        for (ContactEntry entry : uploaded) {
            auto it = directory.find(entry.getPhone());
            if (it != directory.end() && !it->second.getDisplayName().empty()) {
                entry.setDirectoryName(it->second.getDisplayName());
            }
            enriched.push_back(entry);
        }
        return enriched;
    }
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_SERVICES_CONTACTSYNCSERVICE_H

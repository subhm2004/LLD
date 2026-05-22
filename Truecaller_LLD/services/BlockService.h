#ifndef TRUECALLER_LLD_SERVICES_BLOCKSERVICE_H
#define TRUECALLER_LLD_SERVICES_BLOCKSERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../utils/PhoneUtils.h"

namespace truecaller_lld {

class BlockService {
public:
    void block(const std::string& userId, const std::string& phone) {
        blocked_[userId].insert(normalizePhone(phone));
    }

    void unblock(const std::string& userId, const std::string& phone) {
        auto it = blocked_.find(userId);
        if (it == blocked_.end()) {
            return;
        }
        it->second.erase(normalizePhone(phone));
    }

    bool isBlocked(const std::string& userId, const std::string& phone) const {
        auto it = blocked_.find(userId);
        if (it == blocked_.end()) {
            return false;
        }
        return it->second.count(normalizePhone(phone)) != 0;
    }

    std::vector<std::string> listBlocked(const std::string& userId) const {
        std::vector<std::string> result;
        auto it = blocked_.find(userId);
        if (it == blocked_.end()) {
            return result;
        }
        result.assign(it->second.begin(), it->second.end());
        return result;
    }

private:
    std::unordered_map<std::string, std::unordered_set<std::string>> blocked_;
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_SERVICES_BLOCKSERVICE_H

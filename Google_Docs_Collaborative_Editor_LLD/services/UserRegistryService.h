#ifndef GOOGLE_DOCS_LLD_SERVICES_USER_REGISTRY_SERVICE_H
#define GOOGLE_DOCS_LLD_SERVICES_USER_REGISTRY_SERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>

#include "../models/User.h"

namespace google_docs_lld {

class UserRegistryService {
public:
    std::string registerUser(const std::string& displayName, const std::string& email) {
        const std::string userId = "usr_" + std::to_string(++counter_);
        users_[userId] = User{userId, displayName, email};
        return userId;
    }

    const User& getUser(const std::string& userId) const {
        auto it = users_.find(userId);
        if (it == users_.end()) {
            throw std::invalid_argument("unknown user: " + userId);
        }
        return it->second;
    }

private:
    std::unordered_map<std::string, User> users_;
    int counter_{0};
};

}  // namespace google_docs_lld

#endif

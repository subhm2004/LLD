#ifndef TRUECALLER_LLD_MODELS_USER_H
#define TRUECALLER_LLD_MODELS_USER_H

#include <string>

#include "../enums/AccountStatus.h"
#include "../utils/PhoneUtils.h"

namespace truecaller_lld {

class User {
public:
    User() = default;

    User(std::string userId, std::string displayName, std::string phone, std::string email = "",
         bool isBusiness = false)
        : userId_(std::move(userId)),
          displayName_(std::move(displayName)),
          phone_(normalizePhone(phone)),
          email_(std::move(email)),
          isBusiness_(isBusiness) {}

    const std::string& getUserId() const { return userId_; }
    const std::string& getDisplayName() const { return displayName_; }
    const std::string& getPhone() const { return phone_; }
    const std::string& getEmail() const { return email_; }
    bool isBusiness() const { return isBusiness_; }
    AccountStatus getStatus() const { return status_; }

    void setDisplayName(const std::string& name) {
        if (name.empty()) {
            throw std::invalid_argument("display name cannot be empty");
        }
        displayName_ = name;
    }

    void setEmail(const std::string& email) { email_ = email; }
    void setBusiness(bool value) { isBusiness_ = value; }
    void setStatus(AccountStatus status) { status_ = status; }

private:
    std::string userId_;
    std::string displayName_;
    std::string phone_;
    std::string email_;
    bool isBusiness_{false};
    AccountStatus status_{AccountStatus::ACTIVE};
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_MODELS_USER_H

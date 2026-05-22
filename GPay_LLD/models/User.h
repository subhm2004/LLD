#ifndef GPAY_LLD_MODELS_USER_H
#define GPAY_LLD_MODELS_USER_H

#include <string>

#include "../utils/UpiUtils.h"

namespace gpay_lld {

class User {
public:
    User() = default;

    User(std::string userId, std::string name, std::string phone, std::string upiId,
         std::string pinHash)
        : userId_(std::move(userId)),
          name_(std::move(name)),
          phone_(normalizePhone(phone)),
          upiId_(std::move(upiId)),
          pinHash_(std::move(pinHash)) {
        validateUpiId(upiId_);
    }

    const std::string& getUserId() const { return userId_; }
    const std::string& getName() const { return name_; }
    const std::string& getPhone() const { return phone_; }
    const std::string& getUpiId() const { return upiId_; }
    const std::string& getPinHash() const { return pinHash_; }

private:
    std::string userId_;
    std::string name_;
    std::string phone_;
    std::string upiId_;
    std::string pinHash_;
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_MODELS_USER_H

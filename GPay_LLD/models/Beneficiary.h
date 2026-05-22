#ifndef GPAY_LLD_MODELS_BENEFICIARY_H
#define GPAY_LLD_MODELS_BENEFICIARY_H

#include <string>

namespace gpay_lld {

class Beneficiary {
public:
    Beneficiary() = default;

    Beneficiary(std::string ownerUserId, std::string nickname, std::string upiId)
        : ownerUserId_(std::move(ownerUserId)),
          nickname_(std::move(nickname)),
          upiId_(std::move(upiId)) {}

    const std::string& getOwnerUserId() const { return ownerUserId_; }
    const std::string& getNickname() const { return nickname_; }
    const std::string& getUpiId() const { return upiId_; }

private:
    std::string ownerUserId_;
    std::string nickname_;
    std::string upiId_;
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_MODELS_BENEFICIARY_H

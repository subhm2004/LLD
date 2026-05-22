#ifndef TRUECALLER_LLD_MODELS_PHONEPROFILE_H
#define TRUECALLER_LLD_MODELS_PHONEPROFILE_H

#include <string>
#include <vector>

#include "../enums/CallerTag.h"
#include "../utils/PhoneUtils.h"

namespace truecaller_lld {

/** Global directory entry for a phone number. */
class PhoneProfile {
public:
    PhoneProfile() = default;

    explicit PhoneProfile(std::string phone) : phone_(normalizePhone(std::move(phone))) {}

    PhoneProfile(std::string phone, std::string displayName, std::string ownerUserId, CallerTag tag,
                 int spamScore = 0)
        : phone_(normalizePhone(std::move(phone))),
          displayName_(std::move(displayName)),
          ownerUserId_(std::move(ownerUserId)),
          primaryTag_(tag),
          spamScore_(spamScore) {}

    const std::string& getPhone() const { return phone_; }
    const std::string& getDisplayName() const { return displayName_; }
    const std::string& getOwnerUserId() const { return ownerUserId_; }
    CallerTag getPrimaryTag() const { return primaryTag_; }
    int getSpamScore() const { return spamScore_; }
    int getReportCount() const { return reportCount_; }

    bool isSpam() const { return spamScore_ >= 70 || primaryTag_ == CallerTag::SPAM; }

    void setDisplayName(const std::string& name) { displayName_ = name; }
    void setPrimaryTag(CallerTag tag) { primaryTag_ = tag; }
    void addSpamReport(int scoreDelta) {
        reportCount_++;
        spamScore_ = std::min(100, spamScore_ + scoreDelta);
        if (spamScore_ >= 70) {
            primaryTag_ = CallerTag::SPAM;
        }
    }

private:
    std::string phone_;
    std::string displayName_;
    std::string ownerUserId_;
    CallerTag primaryTag_{CallerTag::UNKNOWN};
    int spamScore_{0};
    int reportCount_{0};
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_MODELS_PHONEPROFILE_H

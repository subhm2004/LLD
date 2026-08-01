#ifndef AMAZON_LOCKER_SERVICE_LLD_MODELS_ACCESSCODE_H
#define AMAZON_LOCKER_SERVICE_LLD_MODELS_ACCESSCODE_H

#include <string>

#include "../enums/AccessCodeStatus.h"

namespace amazon_locker_lld {

class AccessCode {
public:
    AccessCode(std::string codeId, std::string code, std::string packageId, std::string compartmentId,
               long long createdAtEpoch, long long expiresAtEpoch, int maxAttempts)
        : codeId_(std::move(codeId)),
          code_(std::move(code)),
          packageId_(std::move(packageId)),
          compartmentId_(std::move(compartmentId)),
          createdAtEpoch_(createdAtEpoch),
          expiresAtEpoch_(expiresAtEpoch),
          maxAttempts_(maxAttempts),
          failedAttempts_(0),
          status_(AccessCodeStatus::ACTIVE) {}

    const std::string& codeId() const { return codeId_; }
    const std::string& code() const { return code_; }
    const std::string& packageId() const { return packageId_; }
    const std::string& compartmentId() const { return compartmentId_; }
    long long expiresAtEpoch() const { return expiresAtEpoch_; }
    AccessCodeStatus status() const { return status_; }
    int failedAttempts() const { return failedAttempts_; }
    int maxAttempts() const { return maxAttempts_; }

    bool isExpired(long long nowEpoch) const {
        return nowEpoch > expiresAtEpoch_ || status_ == AccessCodeStatus::EXPIRED;
    }

    void recordFailedAttempt(long long nowEpoch) {
        ++failedAttempts_;
        if (failedAttempts_ >= maxAttempts_) {
            status_ = AccessCodeStatus::LOCKED;
        }
        if (nowEpoch > expiresAtEpoch_) {
            status_ = AccessCodeStatus::EXPIRED;
        }
    }

    void markUsed() { status_ = AccessCodeStatus::USED; }
    void markExpired() { status_ = AccessCodeStatus::EXPIRED; }

private:
    std::string codeId_;
    std::string code_;
    std::string packageId_;
    std::string compartmentId_;
    long long createdAtEpoch_;
    long long expiresAtEpoch_;
    int maxAttempts_;
    int failedAttempts_;
    AccessCodeStatus status_;
};

}  // namespace amazon_locker_lld

#endif  // AMAZON_LOCKER_SERVICE_LLD_MODELS_ACCESSCODE_H

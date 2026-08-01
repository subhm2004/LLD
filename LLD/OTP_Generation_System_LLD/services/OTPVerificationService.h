#ifndef OTP_GENERATION_SYSTEM_LLD_SERVICES_OTPVERIFICATIONSERVICE_H
#define OTP_GENERATION_SYSTEM_LLD_SERVICES_OTPVERIFICATIONSERVICE_H

#include <stdexcept>
#include <string>

#include "../enums/OTPStatus.h"
#include "../models/OTPRecord.h"
#include "OTPStorageService.h"

namespace otp_lld {

class OTPVerificationService {
public:
    explicit OTPVerificationService(OTPStorageService &storage) : storage_(storage) {}

    bool verify(const std::string &otpId, const std::string &enteredOtp, long long nowEpoch) {
        OTPRecord *record = storage_.findById(otpId);
        if (record == nullptr) {
            throw std::runtime_error("OTP session not found: " + otpId);
        }

        if (record->status == OTPStatus::VERIFIED) {
            throw std::runtime_error("OTP already used");
        }
        if (record->status == OTPStatus::LOCKED) {
            throw std::runtime_error("OTP locked due to max failed attempts");
        }
        if (record->isExpired(nowEpoch)) {
            record->status = OTPStatus::EXPIRED;
            storage_.update(*record);
            throw std::runtime_error("OTP expired");
        }

        if (enteredOtp == record->otpCode) {
            record->status = OTPStatus::VERIFIED;
            storage_.update(*record);
            return true;
        }

        --record->attemptsRemaining;
        if (record->attemptsRemaining <= 0) {
            record->status = OTPStatus::LOCKED;
        }
        storage_.update(*record);
        return false;
    }

private:
    OTPStorageService &storage_;
};

} // namespace otp_lld

#endif // OTP_GENERATION_SYSTEM_LLD_SERVICES_OTPVERIFICATIONSERVICE_H

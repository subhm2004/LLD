#ifndef OTP_GENERATION_SYSTEM_LLD_MODELS_OTPRECORD_H
#define OTP_GENERATION_SYSTEM_LLD_MODELS_OTPRECORD_H

#include <string>

#include "../enums/OTPChannel.h"
#include "../enums/OTPPurpose.h"
#include "../enums/OTPStatus.h"

namespace otp_lld {

struct OTPRecord {
    std::string otpId;
    std::string userId;
    std::string destination;
    std::string otpCode;
    OTPChannel channel;
    OTPPurpose purpose;
    OTPStatus status;
    long long createdAtEpoch;
    long long expiresAtEpoch;
    int attemptsRemaining;

    bool isExpired(long long nowEpoch) const { return nowEpoch > expiresAtEpoch; }
};

} // namespace otp_lld

#endif // OTP_GENERATION_SYSTEM_LLD_MODELS_OTPRECORD_H

#ifndef OTP_GENERATION_SYSTEM_LLD_CORE_OTPGENERATIONSYSTEM_H
#define OTP_GENERATION_SYSTEM_LLD_CORE_OTPGENERATIONSYSTEM_H

#include <chrono>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#include "../enums/OTPChannel.h"
#include "../enums/OTPPurpose.h"
#include "../enums/OTPStatus.h"
#include "../models/OTPRecord.h"
#include "../services/NotificationDispatcher.h"
#include "../services/OTPGeneratorService.h"
#include "../services/OTPStorageService.h"
#include "../services/OTPVerificationService.h"
#include "../services/RateLimitService.h"
#include "../strategies/IOTPGeneratorStrategy.h"
#include "../strategies/NumericOTPGenerator.h"

namespace otp_lld {

class OTPGenerationSystem {
public:
    OTPGenerationSystem()
        : generator_(std::make_shared<NumericOTPGenerator>()),
          generatorService_(generator_),
          verificationService_(storage_),
          rateLimitService_(3, 600),
          otpLength_(6),
          validityMinutes_(5),
          maxAttempts_(3),
          otpCounter_(0) {}

    long long nowEpoch() const {
        return std::chrono::duration_cast<std::chrono::seconds>(
                   std::chrono::system_clock::now().time_since_epoch())
            .count();
    }

    std::string sendOtp(const std::string &userId, const std::string &destination, OTPChannel channel,
                        OTPPurpose purpose) {
        validateSendRequest(userId, destination, channel);

        const long long now = nowEpoch();
        rateLimitService_.checkAndRecord(userId, now);

        storage_.invalidatePendingForUser(userId, purpose);

        const std::string otpCode = generatorService_.generateOtp(otpLength_);
        const std::string otpId = "OTP_" + std::to_string(++otpCounter_);

        OTPRecord record{otpId,
                         userId,
                         destination,
                         otpCode,
                         channel,
                         purpose,
                         OTPStatus::PENDING,
                         now,
                         now + validityMinutes_ * 60LL,
                         maxAttempts_};

        storage_.save(record);
        notificationDispatcher_.dispatch(channel, destination, otpCode, validityMinutes_);

        std::cout << "OTP sent | id=" << otpId << " | user=" << userId << " | purpose="
                  << purposeToString(purpose) << " | expires in " << validityMinutes_ << " min\n";
        return otpId;
    }

    bool verifyOtp(const std::string &otpId, const std::string &enteredOtp) {
        if (otpId.empty() || enteredOtp.empty()) {
            throw std::invalid_argument("otpId and enteredOtp are required");
        }

        const bool ok = verificationService_.verify(otpId, enteredOtp, nowEpoch());
        if (ok) {
            std::cout << "OTP verified successfully: " << otpId << "\n";
        } else {
            std::cout << "OTP verification failed (wrong code): " << otpId << "\n";
        }
        return ok;
    }

    std::string resendOtp(const std::string &otpId) {
        OTPRecord *existing = storage_.findById(otpId);
        if (existing == nullptr) {
            throw std::runtime_error("Cannot resend — OTP session not found");
        }
        return sendOtp(existing->userId, existing->destination, existing->channel, existing->purpose);
    }

    // Demo helper only — production me OTP plain text expose mat karo
    std::string getOtpCodeForDemo(const std::string &otpId) const {
        const OTPRecord *record = storage_.findByIdConst(otpId);
        if (record == nullptr) {
            throw std::runtime_error("OTP not found: " + otpId);
        }
        return record->otpCode;
    }

    void printOtpStatus(const std::string &otpId) const {
        const OTPRecord *record = storage_.findByIdConst(otpId);
        if (record == nullptr) {
            std::cout << "No record for " << otpId << "\n";
            return;
        }
        std::cout << "Status | id=" << record->otpId << " user=" << record->userId
                  << " status=" << statusToString(record->status)
                  << " attemptsLeft=" << record->attemptsRemaining
                  << " expired=" << (record->isExpired(nowEpoch()) ? "yes" : "no") << "\n";
    }

private:
    std::shared_ptr<IOTPGeneratorStrategy> generator_;
    OTPGeneratorService generatorService_;
    OTPStorageService storage_;
    OTPVerificationService verificationService_;
    NotificationDispatcher notificationDispatcher_;
    RateLimitService rateLimitService_;

    int otpLength_;
    int validityMinutes_;
    int maxAttempts_;
    int otpCounter_;

    static void validateSendRequest(const std::string &userId, const std::string &destination,
                                    OTPChannel channel) {
        if (userId.empty()) {
            throw std::invalid_argument("userId cannot be empty");
        }
        if (destination.empty()) {
            throw std::invalid_argument("destination cannot be empty");
        }
        if (channel != OTPChannel::SMS && channel != OTPChannel::EMAIL) {
            throw std::invalid_argument("unsupported OTP channel");
        }
    }

    static std::string purposeToString(OTPPurpose purpose) {
        switch (purpose) {
        case OTPPurpose::LOGIN:
            return "LOGIN";
        case OTPPurpose::SIGNUP:
            return "SIGNUP";
        case OTPPurpose::PAYMENT:
            return "PAYMENT";
        case OTPPurpose::PASSWORD_RESET:
            return "PASSWORD_RESET";
        }
        return "UNKNOWN";
    }

    static std::string statusToString(OTPStatus status) {
        switch (status) {
        case OTPStatus::PENDING:
            return "PENDING";
        case OTPStatus::VERIFIED:
            return "VERIFIED";
        case OTPStatus::EXPIRED:
            return "EXPIRED";
        case OTPStatus::LOCKED:
            return "LOCKED";
        }
        return "UNKNOWN";
    }
};

} // namespace otp_lld

#endif // OTP_GENERATION_SYSTEM_LLD_CORE_OTPGENERATIONSYSTEM_H

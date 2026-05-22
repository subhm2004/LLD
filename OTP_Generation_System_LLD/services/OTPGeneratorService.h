#ifndef OTP_GENERATION_SYSTEM_LLD_SERVICES_OTPGENERATORSERVICE_H
#define OTP_GENERATION_SYSTEM_LLD_SERVICES_OTPGENERATORSERVICE_H

#include <memory>
#include <string>

#include "../strategies/IOTPGeneratorStrategy.h"

namespace otp_lld {

class OTPGeneratorService {
public:
    explicit OTPGeneratorService(std::shared_ptr<IOTPGeneratorStrategy> strategy) : strategy_(std::move(strategy)) {}

    std::string generateOtp(int length = 6) const { return strategy_->generate(length); }

private:
    std::shared_ptr<IOTPGeneratorStrategy> strategy_;
};

} // namespace otp_lld

#endif // OTP_GENERATION_SYSTEM_LLD_SERVICES_OTPGENERATORSERVICE_H

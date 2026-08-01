#ifndef OTP_GENERATION_SYSTEM_LLD_STRATEGIES_ALPHANUMERICOTPGENERATOR_H
#define OTP_GENERATION_SYSTEM_LLD_STRATEGIES_ALPHANUMERICOTPGENERATOR_H

#include <random>
#include <string>

#include "IOTPGeneratorStrategy.h"

namespace otp_lld {

class AlphanumericOTPGenerator : public IOTPGeneratorStrategy {
public:
    std::string generate(int length) const override {
        static const std::string charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::string otp;
        otp.reserve(static_cast<std::size_t>(length));
        for (int i = 0; i < length; ++i) {
            otp.push_back(charset[charDist_(rng_)]);
        }
        return otp;
    }

private:
    mutable std::mt19937 rng_{std::random_device{}()};
    mutable std::uniform_int_distribution<int> charDist_{0, 35};
};

} // namespace otp_lld

#endif // OTP_GENERATION_SYSTEM_LLD_STRATEGIES_ALPHANUMERICOTPGENERATOR_H

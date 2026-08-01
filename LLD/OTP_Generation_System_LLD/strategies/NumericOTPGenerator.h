#ifndef OTP_GENERATION_SYSTEM_LLD_STRATEGIES_NUMERICOTPGENERATOR_H
#define OTP_GENERATION_SYSTEM_LLD_STRATEGIES_NUMERICOTPGENERATOR_H

#include <random>
#include <stdexcept>
#include <string>

#include "IOTPGeneratorStrategy.h"

namespace otp_lld {

class NumericOTPGenerator : public IOTPGeneratorStrategy {
public:
    std::string generate(int length) const override {
        if (length <= 0 || length > 10) {
            throw std::invalid_argument("Numeric OTP length must be 1-10");
        }
        std::string otp;
        otp.reserve(static_cast<std::size_t>(length));
        for (int i = 0; i < length; ++i) {
            otp.push_back(static_cast<char>('0' + digitDist_(rng_)));
        }
        return otp;
    }

private:
    mutable std::mt19937 rng_{std::random_device{}()};
    mutable std::uniform_int_distribution<int> digitDist_{0, 9};
};

} // namespace otp_lld

#endif // OTP_GENERATION_SYSTEM_LLD_STRATEGIES_NUMERICOTPGENERATOR_H

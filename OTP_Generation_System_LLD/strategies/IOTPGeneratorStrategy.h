#ifndef OTP_GENERATION_SYSTEM_LLD_STRATEGIES_IOTPGENERATORSTRATEGY_H
#define OTP_GENERATION_SYSTEM_LLD_STRATEGIES_IOTPGENERATORSTRATEGY_H

#include <string>

namespace otp_lld {

class IOTPGeneratorStrategy {
public:
    virtual ~IOTPGeneratorStrategy() = default;
    virtual std::string generate(int length) const = 0;
};

} // namespace otp_lld

#endif // OTP_GENERATION_SYSTEM_LLD_STRATEGIES_IOTPGENERATORSTRATEGY_H

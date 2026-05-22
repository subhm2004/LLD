#ifndef OTP_GENERATION_SYSTEM_LLD_CHANNELS_SMSCHANNEL_H
#define OTP_GENERATION_SYSTEM_LLD_CHANNELS_SMSCHANNEL_H

#include <iostream>
#include <string>

#include "INotificationChannel.h"

namespace otp_lld {

class SMSChannel : public INotificationChannel {
public:
    OTPChannel getChannelType() const override { return OTPChannel::SMS; }

    void send(const std::string &destination, const std::string &message) const override {
        std::cout << "[SMS -> " << destination << "] " << message << "\n";
    }
};

} // namespace otp_lld

#endif // OTP_GENERATION_SYSTEM_LLD_CHANNELS_SMSCHANNEL_H

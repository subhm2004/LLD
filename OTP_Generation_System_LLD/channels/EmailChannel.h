#ifndef OTP_GENERATION_SYSTEM_LLD_CHANNELS_EMAILCHANNEL_H
#define OTP_GENERATION_SYSTEM_LLD_CHANNELS_EMAILCHANNEL_H

#include <iostream>
#include <string>

#include "INotificationChannel.h"

namespace otp_lld {

class EmailChannel : public INotificationChannel {
public:
    OTPChannel getChannelType() const override { return OTPChannel::EMAIL; }

    void send(const std::string &destination, const std::string &message) const override {
        std::cout << "[EMAIL -> " << destination << "] " << message << "\n";
    }
};

} // namespace otp_lld

#endif // OTP_GENERATION_SYSTEM_LLD_CHANNELS_EMAILCHANNEL_H

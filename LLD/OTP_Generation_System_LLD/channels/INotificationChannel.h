#ifndef OTP_GENERATION_SYSTEM_LLD_CHANNELS_INOTIFICATIONCHANNEL_H
#define OTP_GENERATION_SYSTEM_LLD_CHANNELS_INOTIFICATIONCHANNEL_H

#include <string>

#include "../enums/OTPChannel.h"

namespace otp_lld {

class INotificationChannel {
public:
    virtual ~INotificationChannel() = default;
    virtual OTPChannel getChannelType() const = 0;
    virtual void send(const std::string &destination, const std::string &message) const = 0;
};

} // namespace otp_lld

#endif // OTP_GENERATION_SYSTEM_LLD_CHANNELS_INOTIFICATIONCHANNEL_H

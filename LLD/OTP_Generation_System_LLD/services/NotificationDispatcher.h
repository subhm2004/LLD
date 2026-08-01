#ifndef OTP_GENERATION_SYSTEM_LLD_SERVICES_NOTIFICATIONDISPATCHER_H
#define OTP_GENERATION_SYSTEM_LLD_SERVICES_NOTIFICATIONDISPATCHER_H

#include <memory>
#include <stdexcept>
#include <unordered_map>

#include "../channels/EmailChannel.h"
#include "../channels/INotificationChannel.h"
#include "../channels/SMSChannel.h"
#include "../enums/OTPChannel.h"

namespace otp_lld {

class NotificationDispatcher {
public:
    NotificationDispatcher() {
        channels_[OTPChannel::SMS] = std::make_shared<SMSChannel>();
        channels_[OTPChannel::EMAIL] = std::make_shared<EmailChannel>();
    }

    void dispatch(OTPChannel channel, const std::string &destination, const std::string &otpCode,
                    int validityMinutes) const {
        auto it = channels_.find(channel);
        if (it == channels_.end()) {
            throw std::runtime_error("Notification channel not configured");
        }
        const std::string message =
            "Your OTP is " + otpCode + ". Valid for " + std::to_string(validityMinutes) + " minutes.";
        it->second->send(destination, message);
    }

private:
    std::unordered_map<OTPChannel, std::shared_ptr<INotificationChannel>> channels_;
};

} // namespace otp_lld

#endif // OTP_GENERATION_SYSTEM_LLD_SERVICES_NOTIFICATIONDISPATCHER_H

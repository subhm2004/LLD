#ifndef OTP_GENERATION_SYSTEM_LLD_SERVICES_RATELIMITSERVICE_H
#define OTP_GENERATION_SYSTEM_LLD_SERVICES_RATELIMITSERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>

namespace otp_lld {

class RateLimitService {
public:
    RateLimitService(int maxRequests, long long windowSeconds)
        : maxRequests_(maxRequests), windowSeconds_(windowSeconds) {}

    void checkAndRecord(const std::string &userId, long long nowEpoch) {
        WindowState &state = windows_[userId];
        if (nowEpoch - state.windowStartEpoch >= windowSeconds_) {
            state.windowStartEpoch = nowEpoch;
            state.count = 0;
        }
        if (state.count >= maxRequests_) {
            throw std::runtime_error("OTP rate limit exceeded for user: " + userId);
        }
        ++state.count;
    }

private:
    struct WindowState {
        long long windowStartEpoch = 0;
        int count = 0;
    };

    int maxRequests_;
    long long windowSeconds_;
    std::unordered_map<std::string, WindowState> windows_;
};

} // namespace otp_lld

#endif // OTP_GENERATION_SYSTEM_LLD_SERVICES_RATELIMITSERVICE_H

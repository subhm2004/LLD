#ifndef UBER_LLD_SERVICES_OTPSERVICE_H
#define UBER_LLD_SERVICES_OTPSERVICE_H

#include <bits/stdc++.h>

using namespace std;

namespace uber_lld {

class OtpService {
public:
    int generateOtp(const string &rideId) {
        const int otp = 1000 + (otpCounter_ % 9000);
        ++otpCounter_;
        otpByRide_[rideId] = otp;
        return otp;
    }

    bool verifyOtp(const string &rideId, int otp) {
        auto it = otpByRide_.find(rideId);
        if (it == otpByRide_.end()) {
            return false;
        }
        if (it->second != otp) {
            return false;
        }
        otpByRide_.erase(it);
        return true;
    }

private:
    unordered_map<string, int> otpByRide_;
    int otpCounter_ = 1234;
};

} // namespace uber_lld

#endif // UBER_LLD_SERVICES_OTPSERVICE_H

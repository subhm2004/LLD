#ifndef GPAY_LLD_SERVICES_PINAUTHSERVICE_H
#define GPAY_LLD_SERVICES_PINAUTHSERVICE_H

#include <stdexcept>
#include <string>

#include "../models/User.h"

namespace gpay_lld {

class PinAuthService {
public:
    static std::string hashPin(const std::string& pin) { return "hash_" + pin; }

    void verifyPin(const User& user, const std::string& pin) const {
        if (pin.empty()) {
            throw std::invalid_argument("PIN required");
        }
        if (hashPin(pin) != user.getPinHash()) {
            throw std::runtime_error("invalid PIN");
        }
    }
};

}  // namespace gpay_lld

#endif  // GPAY_LLD_SERVICES_PINAUTHSERVICE_H

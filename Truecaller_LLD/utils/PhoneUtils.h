#ifndef TRUECALLER_LLD_UTILS_PHONEUTILS_H
#define TRUECALLER_LLD_UTILS_PHONEUTILS_H

#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <string>

namespace truecaller_lld {

inline std::string normalizePhone(const std::string& raw) {
    if (raw.empty()) {
        throw std::invalid_argument("phone number cannot be empty");
    }
    std::string digits;
    for (char c : raw) {
        if (std::isdigit(static_cast<unsigned char>(c)) != 0) {
            digits.push_back(c);
        }
    }
    if (digits.size() < 10) {
        throw std::invalid_argument("invalid phone number: " + raw);
    }
    if (digits.size() == 10) {
        return "+91" + digits;
    }
    return "+" + digits;
}

inline std::string toLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_UTILS_PHONEUTILS_H

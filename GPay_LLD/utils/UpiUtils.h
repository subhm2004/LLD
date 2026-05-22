#ifndef GPAY_LLD_UTILS_UPIUTILS_H
#define GPAY_LLD_UTILS_UPIUTILS_H

#include <regex>
#include <stdexcept>
#include <string>

namespace gpay_lld {

inline bool isValidUpiId(const std::string& upiId) {
    static const std::regex pattern(R"(^[a-zA-Z0-9.\-_]{2,256}@[a-zA-Z]{2,64}$)");
    return std::regex_match(upiId, pattern);
}

inline void validateUpiId(const std::string& upiId) {
    if (!isValidUpiId(upiId)) {
        throw std::invalid_argument("invalid UPI ID: " + upiId);
    }
}

inline std::string normalizePhone(const std::string& raw) {
    std::string digits;
    for (char c : raw) {
        if (c >= '0' && c <= '9') {
            digits.push_back(c);
        }
    }
    if (digits.size() == 10) {
        return "+91" + digits;
    }
    if (digits.size() >= 11) {
        return "+" + digits;
    }
    throw std::invalid_argument("invalid phone: " + raw);
}

/** Parse demo QR: upi://pay?pa=merchant@okaxis&am=250&tn=Tea */
struct QrPaymentData {
    std::string payeeUpi;
    double amount{0};
    std::string note;
};

inline QrPaymentData parseUpiQr(const std::string& qrPayload) {
    QrPaymentData data;
    const auto paPos = qrPayload.find("pa=");
    const auto amPos = qrPayload.find("am=");
    const auto tnPos = qrPayload.find("tn=");

    if (paPos == std::string::npos || amPos == std::string::npos) {
        throw std::invalid_argument("invalid UPI QR payload");
    }

    size_t paStart = paPos + 3;
    size_t paEnd = qrPayload.find('&', paStart);
    data.payeeUpi = qrPayload.substr(paStart, paEnd - paStart);
    validateUpiId(data.payeeUpi);

    size_t amStart = amPos + 3;
    size_t amEnd = qrPayload.find('&', amStart);
    std::string amountStr = qrPayload.substr(amStart, amEnd - amStart);
    data.amount = std::stod(amountStr);
    if (data.amount <= 0) {
        throw std::invalid_argument("QR amount must be positive");
    }

    if (tnPos != std::string::npos) {
        size_t tnStart = tnPos + 3;
        data.note = qrPayload.substr(tnStart);
    }
    return data;
}

}  // namespace gpay_lld

#endif  // GPAY_LLD_UTILS_UPIUTILS_H

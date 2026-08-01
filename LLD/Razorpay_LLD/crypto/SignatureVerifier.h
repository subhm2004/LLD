#ifndef RAZORPAY_LLD_CRYPTO_SIGNATURE_VERIFIER_H
#define RAZORPAY_LLD_CRYPTO_SIGNATURE_VERIFIER_H

#include <sstream>
#include <string>

namespace razorpay_lld {

// Demo HMAC stand-in — production: OpenSSL HMAC-SHA256(webhook_secret, body)
class SignatureVerifier {
public:
    static std::string computeSignature(const std::string& payload, const std::string& secret) {
        std::ostringstream oss;
        oss << "rzp_sig_" << secret.size() << "_" << payload.size();
        for (char c : payload) {
            oss << static_cast<int>(c % 17);
        }
        return oss.str();
    }

    static bool verify(const std::string& payload, const std::string& secret,
                       const std::string& signature) {
        return computeSignature(payload, secret) == signature;
    }
};

}  // namespace razorpay_lld

#endif

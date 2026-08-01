#ifndef RAZORPAY_LLD_STRATEGIES_I_PAYMENT_METHOD_VALIDATOR_H
#define RAZORPAY_LLD_STRATEGIES_I_PAYMENT_METHOD_VALIDATOR_H

#include <cctype>
#include <stdexcept>
#include <string>

#include "../enums/PaymentMethod.h"

namespace razorpay_lld {

class IPaymentMethodValidator {
public:
    virtual ~IPaymentMethodValidator() = default;
    virtual void validate(const std::string& instrumentDetail) const = 0;
    virtual PaymentMethod method() const = 0;
};

class UpiValidator : public IPaymentMethodValidator {
public:
    void validate(const std::string& vpa) const override {
        if (vpa.empty() || vpa.find('@') == std::string::npos) {
            throw std::invalid_argument("invalid UPI VPA — expected user@bank");
        }
        const auto at = vpa.find('@');
        if (at == 0 || at == vpa.size() - 1) {
            throw std::invalid_argument("invalid UPI VPA format");
        }
    }
    PaymentMethod method() const override { return PaymentMethod::UPI; }
};

// instrumentDetail format: cardNumber|MM/YY|CVV  (16-digit card + expiry + CVV)
class CardValidator : public IPaymentMethodValidator {
public:
    void validate(const std::string& instrumentDetail) const override {
        const auto p1 = instrumentDetail.find('|');
        const auto p2 = instrumentDetail.find('|', p1 == std::string::npos ? 0 : p1 + 1);
        if (p1 == std::string::npos || p2 == std::string::npos) {
            throw std::invalid_argument("card format: 16-digit|MM/YY|CVV");
        }

        const std::string cardNumber = instrumentDetail.substr(0, p1);
        const std::string expiry = instrumentDetail.substr(p1 + 1, p2 - p1 - 1);
        const std::string cvv = instrumentDetail.substr(p2 + 1);

        if (cardNumber.size() != 16) {
            throw std::invalid_argument("card number must be 16 digits");
        }
        for (char c : cardNumber) {
            if (!std::isdigit(static_cast<unsigned char>(c))) {
                throw std::invalid_argument("card number must be numeric");
            }
        }

        if (expiry.size() != 5 || expiry[2] != '/') {
            throw std::invalid_argument("expiry must be MM/YY");
        }
        for (size_t i = 0; i < expiry.size(); ++i) {
            if (i == 2) continue;
            if (!std::isdigit(static_cast<unsigned char>(expiry[i]))) {
                throw std::invalid_argument("expiry must be MM/YY digits");
            }
        }

        if (cvv.size() < 3 || cvv.size() > 4) {
            throw std::invalid_argument("CVV must be 3 or 4 digits");
        }
        for (char c : cvv) {
            if (!std::isdigit(static_cast<unsigned char>(c))) {
                throw std::invalid_argument("CVV must be numeric");
            }
        }
    }
    PaymentMethod method() const override { return PaymentMethod::CARD; }
};

// instrumentDetail = wallet provider name (paytm, phonepe, amazonpay, …)
class WalletValidator : public IPaymentMethodValidator {
public:
    void validate(const std::string& providerName) const override {
        if (providerName.empty()) {
            throw std::invalid_argument("wallet provider name required");
        }
        for (char c : providerName) {
            if (!std::isalnum(static_cast<unsigned char>(c))) {
                throw std::invalid_argument("wallet provider must be alphanumeric");
            }
        }
    }
    PaymentMethod method() const override { return PaymentMethod::WALLET; }
};

}  // namespace razorpay_lld

#endif

#ifndef RAZORPAY_LLD_FACTORIES_PAYMENT_METHOD_VALIDATOR_FACTORY_H
#define RAZORPAY_LLD_FACTORIES_PAYMENT_METHOD_VALIDATOR_FACTORY_H

#include <memory>
#include <stdexcept>

#include "../enums/PaymentMethod.h"
#include "../strategies/IPaymentMethodValidator.h"

namespace razorpay_lld {

class PaymentMethodValidatorFactory {
public:
    static std::unique_ptr<IPaymentMethodValidator> create(PaymentMethod method) {
        switch (method) {
            case PaymentMethod::UPI:
                return std::make_unique<UpiValidator>();
            case PaymentMethod::CARD:
                return std::make_unique<CardValidator>();
            case PaymentMethod::WALLET:
                return std::make_unique<WalletValidator>();
            default:
                throw std::invalid_argument("unsupported payment method");
        }
    }
};

}  // namespace razorpay_lld

#endif

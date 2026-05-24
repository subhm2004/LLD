#ifndef RAZORPAY_LLD_MODELS_MERCHANT_CONFIG_H
#define RAZORPAY_LLD_MODELS_MERCHANT_CONFIG_H

#include <string>

namespace razorpay_lld {

struct MerchantConfig {
    std::string keyId;
    std::string keySecret;
    std::string webhookSecret;
};

}  // namespace razorpay_lld

#endif

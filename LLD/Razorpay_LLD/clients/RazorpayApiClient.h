#ifndef RAZORPAY_LLD_CLIENTS_RAZORPAY_API_CLIENT_H
#define RAZORPAY_LLD_CLIENTS_RAZORPAY_API_CLIENT_H

#include <iostream>
#include <string>

#include "../models/MerchantConfig.h"

namespace razorpay_lld {

// Mock Razorpay REST client — logs API calls instead of HTTP
class RazorpayApiClient {
public:
    explicit RazorpayApiClient(MerchantConfig config) : config_(std::move(config)) {}

    const MerchantConfig& config() const { return config_; }

    void postOrder(const std::string& orderId, int amountPaise, const std::string& currency) const {
        std::cout << "  [API] POST /v1/orders " << orderId << " amount=" << amountPaise << " "
                  << currency << " key=" << config_.keyId << "\n";
    }

    void postPayment(const std::string& paymentId, const std::string& orderId,
                     const std::string& method) const {
        std::cout << "  [API] POST /v1/payments " << paymentId << " order=" << orderId
                  << " method=" << method << "\n";
    }

    void postCapture(const std::string& paymentId, int amountPaise) const {
        std::cout << "  [API] POST /v1/payments/" << paymentId << "/capture amount=" << amountPaise
                  << "\n";
    }

    void postRefund(const std::string& refundId, const std::string& paymentId,
                    int amountPaise) const {
        std::cout << "  [API] POST /v1/payments/" << paymentId << "/refund " << refundId
                  << " amount=" << amountPaise << "\n";
    }

private:
    MerchantConfig config_;
};

}  // namespace razorpay_lld

#endif

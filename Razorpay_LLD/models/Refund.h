#ifndef RAZORPAY_LLD_MODELS_REFUND_H
#define RAZORPAY_LLD_MODELS_REFUND_H

#include <string>

namespace razorpay_lld {

enum class RefundStatus { CREATED, PROCESSED, FAILED };

inline std::string refundStatusToString(RefundStatus status) {
    switch (status) {
        case RefundStatus::CREATED:
            return "created";
        case RefundStatus::PROCESSED:
            return "processed";
        case RefundStatus::FAILED:
            return "failed";
        default:
            return "unknown";
    }
}

struct Refund {
    std::string refundId;
    std::string paymentId;
    int amountPaise{0};
    RefundStatus status{RefundStatus::CREATED};
};

}  // namespace razorpay_lld

#endif

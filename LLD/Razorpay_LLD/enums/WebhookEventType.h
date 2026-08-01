#ifndef RAZORPAY_LLD_ENUMS_WEBHOOK_EVENT_TYPE_H
#define RAZORPAY_LLD_ENUMS_WEBHOOK_EVENT_TYPE_H

#include <string>

namespace razorpay_lld {

enum class WebhookEventType {
    PAYMENT_AUTHORIZED,
    PAYMENT_CAPTURED,
    PAYMENT_FAILED,
    REFUND_PROCESSED,
    UNKNOWN
};

inline WebhookEventType parseWebhookEvent(const std::string& event) {
    if (event == "payment.authorized") return WebhookEventType::PAYMENT_AUTHORIZED;
    if (event == "payment.captured") return WebhookEventType::PAYMENT_CAPTURED;
    if (event == "payment.failed") return WebhookEventType::PAYMENT_FAILED;
    if (event == "refund.processed") return WebhookEventType::REFUND_PROCESSED;
    return WebhookEventType::UNKNOWN;
}

inline std::string webhookEventToString(WebhookEventType type) {
    switch (type) {
        case WebhookEventType::PAYMENT_AUTHORIZED:
            return "payment.authorized";
        case WebhookEventType::PAYMENT_CAPTURED:
            return "payment.captured";
        case WebhookEventType::PAYMENT_FAILED:
            return "payment.failed";
        case WebhookEventType::REFUND_PROCESSED:
            return "refund.processed";
        default:
            return "unknown";
    }
}

}  // namespace razorpay_lld

#endif

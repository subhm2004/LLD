#ifndef RAZORPAY_LLD_SERVICES_WEBHOOK_SERVICE_H
#define RAZORPAY_LLD_SERVICES_WEBHOOK_SERVICE_H

#include <iostream>
#include <stdexcept>
#include <string>

#include "../crypto/SignatureVerifier.h"
#include "../enums/PaymentStatus.h"
#include "../enums/WebhookEventType.h"
#include "../models/MerchantConfig.h"
#include "CaptureService.h"
#include "PaymentService.h"
#include "RefundService.h"

namespace razorpay_lld {

class WebhookService {
public:
    WebhookService(const MerchantConfig& config, PaymentService* payments,
                   CaptureService* capture, RefundService* refunds)
        : config_(config), payments_(payments), capture_(capture), refunds_(refunds) {}

    void handle(const std::string& rawPayload, const std::string& signatureHeader) {
        if (!SignatureVerifier::verify(rawPayload, config_.webhookSecret, signatureHeader)) {
            throw std::runtime_error("invalid webhook signature — state not updated");
        }

        const std::string event = extractField(rawPayload, "event");
        const std::string paymentId = extractField(rawPayload, "payment_id");
        const std::string refundId = extractField(rawPayload, "refund_id");
        const WebhookEventType type = parseWebhookEvent(event);

        std::cout << "  [Webhook] verified " << event;
        if (!paymentId.empty()) std::cout << " payment=" << paymentId;
        if (!refundId.empty()) std::cout << " refund=" << refundId;
        std::cout << "\n";

        switch (type) {
            case WebhookEventType::PAYMENT_AUTHORIZED:
                payments_->markAuthorized(paymentId);
                break;
            case WebhookEventType::PAYMENT_CAPTURED: {
                const RazorpayPayment& payment = payments_->getPayment(paymentId);
                if (payment.status == PaymentStatus::AUTHORIZED) {
                    capture_->capture(paymentId, payment.amountPaise);
                }
                break;
            }
            case WebhookEventType::PAYMENT_FAILED:
                payments_->markFailed(paymentId, "webhook: payment.failed");
                break;
            case WebhookEventType::REFUND_PROCESSED:
                refunds_->markProcessed(refundId);
                break;
            default:
                throw std::runtime_error("unsupported webhook event: " + event);
        }
    }

    static std::string buildPayload(const std::string& event, const std::string& paymentId = "",
                                    const std::string& refundId = "") {
        std::string json = "{\"event\":\"" + event + "\"";
        if (!paymentId.empty()) {
            json += ",\"payment_id\":\"" + paymentId + "\"";
        }
        if (!refundId.empty()) {
            json += ",\"refund_id\":\"" + refundId + "\"";
        }
        json += "}";
        return json;
    }

    static std::string signPayload(const std::string& payload, const std::string& webhookSecret) {
        return SignatureVerifier::computeSignature(payload, webhookSecret);
    }

private:
    static std::string extractField(const std::string& json, const std::string& key) {
        const std::string needle = "\"" + key + "\":\"";
        const auto pos = json.find(needle);
        if (pos == std::string::npos) {
            return "";
        }
        const auto start = pos + needle.size();
        const auto end = json.find('"', start);
        return json.substr(start, end - start);
    }

    MerchantConfig config_;
    PaymentService* payments_;
    CaptureService* capture_;
    RefundService* refunds_;
};

}  // namespace razorpay_lld

#endif

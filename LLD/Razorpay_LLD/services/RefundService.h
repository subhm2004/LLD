#ifndef RAZORPAY_LLD_SERVICES_REFUND_SERVICE_H
#define RAZORPAY_LLD_SERVICES_REFUND_SERVICE_H

#include <stdexcept>
#include <unordered_map>

#include "../clients/RazorpayApiClient.h"
#include "../enums/PaymentStatus.h"
#include "../factories/EntityIdFactory.h"
#include "../models/Refund.h"
#include "PaymentService.h"

namespace razorpay_lld {

class RefundService {
public:
    RefundService(RazorpayApiClient* client, PaymentService* payments)
        : client_(client), payments_(payments) {}

    Refund createRefund(const std::string& paymentId, int amountPaise) {
        const RazorpayPayment& payment = payments_->getPayment(paymentId);
        if (payment.status != PaymentStatus::CAPTURED &&
            payment.status != PaymentStatus::REFUNDED) {
            throw std::runtime_error("refund allowed only on captured payments");
        }
        const int remaining = payment.capturedAmountPaise - payment.refundedAmountPaise;
        if (amountPaise <= 0 || amountPaise > remaining) {
            throw std::invalid_argument("invalid refund amount — exceeds captured balance");
        }

        Refund refund;
        refund.refundId = EntityIdFactory::nextRefundId();
        refund.paymentId = paymentId;
        refund.amountPaise = amountPaise;
        refund.status = RefundStatus::CREATED;

        client_->postRefund(refund.refundId, paymentId, amountPaise);
        refund.status = RefundStatus::PROCESSED;
        payments_->applyRefund(paymentId, amountPaise);
        refunds_[refund.refundId] = refund;
        return refund;
    }

    void markProcessed(const std::string& refundId) {
        Refund& refund = getRefundMutable(refundId);
        refund.status = RefundStatus::PROCESSED;
    }

    void markFailed(const std::string& refundId) {
        Refund& refund = getRefundMutable(refundId);
        refund.status = RefundStatus::FAILED;
    }

    const Refund& getRefund(const std::string& refundId) const {
        auto it = refunds_.find(refundId);
        if (it == refunds_.end()) {
            throw std::runtime_error("refund not found: " + refundId);
        }
        return it->second;
    }

private:
    Refund& getRefundMutable(const std::string& refundId) {
        auto it = refunds_.find(refundId);
        if (it == refunds_.end()) {
            throw std::runtime_error("refund not found: " + refundId);
        }
        return it->second;
    }

    RazorpayApiClient* client_;
    PaymentService* payments_;
    std::unordered_map<std::string, Refund> refunds_;
};

}  // namespace razorpay_lld

#endif

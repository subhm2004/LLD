#include <functional>
#include <iostream>
#include <string>

#include "core/RazorpayPaymentSystem.h"
#include "enums/PaymentMethod.h"
#include "enums/PaymentStatus.h"
#include "models/CheckoutInput.h"
#include "services/WebhookService.h"

using namespace std;
using namespace razorpay_lld;

static void printOrder(const RazorpayOrder& o) {
    cout << "  Order " << o.orderId << " | " << o.amountPaise << " " << o.currency << " | "
         << orderStatusToString(o.status) << " | receipt=" << o.receipt << "\n";
}

static void printPayment(const RazorpayPayment& p) {
    cout << "  Payment " << p.paymentId << " | order=" << p.orderId << " | "
         << paymentMethodToString(p.method) << " | " << paymentStatusToString(p.status);
    if (p.capturedAmountPaise > 0) cout << " | captured=" << p.capturedAmountPaise;
    if (p.refundedAmountPaise > 0) cout << " | refunded=" << p.refundedAmountPaise;
    if (!p.failureReason.empty()) cout << " | " << p.failureReason;
    cout << "\n";
}

static void printRefund(const Refund& r) {
    cout << "  Refund " << r.refundId << " | pay=" << r.paymentId << " | " << r.amountPaise
         << " paise | " << refundStatusToString(r.status) << "\n";
}

static void expectThrow(const function<void()>& fn, const string& label) {
    try {
        fn();
        cout << "  ERROR [" << label << "]: expected exception\n";
    } catch (const exception& ex) {
        cout << "  OK [" << label << "]: " << ex.what() << "\n";
    }
}

int main() {
    cout << "====== Razorpay Payment Gateway LLD (requirements.md demo) ======\n\n";

    MerchantConfig config{"rzp_test_key_001", "secret_demo_abc", "whsec_demo_xyz"};
    RazorpayPaymentSystem rzp(config);

    // --- §1 Order Management ---
    cout << "--- §1 Order Management ---\n";
    const RazorpayOrder order =
        rzp.createOrder(49900, "INR", "rcpt_inv_1001", "idem_order_1");
    printOrder(order);
    const RazorpayOrder dupOrder =
        rzp.createOrder(49900, "INR", "rcpt_inv_1001", "idem_order_1");
    cout << "  Idempotent order: same id = " << (dupOrder.orderId == order.orderId ? "yes" : "no")
         << "\n";
    expectThrow([&]() { rzp.createOrder(-100, "INR", "bad"); }, "reject negative amount");

    // --- §2 Payment Initiation + §3 Instrument Validation ---
    cout << "\n--- §2 Payment Initiation + §3 Instrument Validation ---\n";
    RazorpayPayment pay = rzp.initiatePayment(order.orderId, PaymentMethod::UPI, "user@okaxis",
                                              true, "idem_pay_1");
    printPayment(pay);
    const RazorpayPayment dupPay =
        rzp.initiatePayment(order.orderId, PaymentMethod::UPI, "user@okaxis", true, "idem_pay_1");
    cout << "  Idempotent payment: same id = " << (dupPay.paymentId == pay.paymentId ? "yes" : "no")
         << "\n";
    expectThrow([&]() { rzp.initiatePayment(order.orderId, PaymentMethod::CARD, "4242"); },
                "invalid CARD format");

    // --- §4 Payment Capture ---
    cout << "\n--- §4 Payment Capture ---\n";
    pay = rzp.capturePayment(pay.paymentId, pay.amountPaise);
    printPayment(pay);
    printOrder(rzp.getOrder(order.orderId));
    expectThrow([&]() { rzp.capturePayment(pay.paymentId, pay.amountPaise); },
                "reject capture on non-authorized");

    // --- §7 Checkout Flow ---
    cout << "\n--- §7 Checkout Flow ---\n";
    CheckoutInput cardCheckout{120000, "INR", "rcpt_sub_42", PaymentMethod::CARD,
                               "4111111111111111|12/28|123"};
    const RazorpayPayment checkoutPay = rzp.checkout(cardCheckout);
    printPayment(checkoutPay);

    // --- §6 Webhook Processing ---
    cout << "\n--- §6 Webhook Processing ---\n";
    const RazorpayOrder order2 = rzp.createOrder(25000, "INR", "rcpt_webhook");
    RazorpayPayment authorized =
        rzp.initiatePayment(order2.orderId, PaymentMethod::WALLET, "phonepe");
    const string payloadAuth =
        WebhookService::buildPayload("payment.authorized", authorized.paymentId);
    rzp.processWebhook(payloadAuth, WebhookService::signPayload(payloadAuth, config.webhookSecret));
    const string payloadCap =
        WebhookService::buildPayload("payment.captured", authorized.paymentId);
    rzp.processWebhook(payloadCap, WebhookService::signPayload(payloadCap, config.webhookSecret));
    printPayment(rzp.getPayment(authorized.paymentId));

    const RazorpayOrder orderFail = rzp.createOrder(10000, "INR", "rcpt_fail");
    RazorpayPayment failedPay = rzp.initiatePayment(
        orderFail.orderId, PaymentMethod::CARD, "4111111111111111|01/20|999", false);
    const string payloadFail =
        WebhookService::buildPayload("payment.failed", failedPay.paymentId);
    rzp.processWebhook(payloadFail,
                       WebhookService::signPayload(payloadFail, config.webhookSecret));
    printPayment(rzp.getPayment(failedPay.paymentId));
    expectThrow(
        [&]() { rzp.processWebhook(payloadFail, "tampered_signature"); },
        "reject invalid webhook signature");

    // --- §5 Refund Management ---
    cout << "\n--- §5 Refund Management ---\n";
    Refund partial = rzp.refundPayment(checkoutPay.paymentId, 50000);
    printRefund(partial);
    printPayment(rzp.getPayment(checkoutPay.paymentId));
    Refund full = rzp.refundPayment(checkoutPay.paymentId, 70000);
    printRefund(full);
    printPayment(rzp.getPayment(checkoutPay.paymentId));
    const string payloadRefund =
        WebhookService::buildPayload("refund.processed", "", full.refundId);
    rzp.processWebhook(payloadRefund,
                       WebhookService::signPayload(payloadRefund, config.webhookSecret));
    printRefund(rzp.getRefund(full.refundId));
    expectThrow([&]() { rzp.refundPayment(checkoutPay.paymentId, 1); },
                "reject over-refund");

    // --- §8 Query and Facade API ---
    cout << "\n--- §8 Query and Facade API ---\n";
    cout << "  getOrder: " << rzp.getOrder(order.orderId).orderId << "\n";
    cout << "  getPayment: " << rzp.getPayment(pay.paymentId).paymentId << "\n";
    cout << "  getRefund: " << rzp.getRefund(full.refundId).refundId << "\n";

    cout << "\nAll requirement sections demonstrated.\n";
    return 0;
}

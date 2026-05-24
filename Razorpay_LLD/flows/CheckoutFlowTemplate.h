#ifndef RAZORPAY_LLD_FLOWS_CHECKOUT_FLOW_TEMPLATE_H
#define RAZORPAY_LLD_FLOWS_CHECKOUT_FLOW_TEMPLATE_H

#include <iostream>
#include <string>

#include "../enums/PaymentMethod.h"
#include "../enums/PaymentStatus.h"
#include "../factories/PaymentMethodValidatorFactory.h"
#include "../models/CheckoutInput.h"
#include "../models/RazorpayOrder.h"
#include "../models/RazorpayPayment.h"
#include "../services/CaptureService.h"
#include "../services/OrderService.h"
#include "../services/PaymentService.h"

namespace razorpay_lld {

// Template Method: validate input → create order → initiate payment → capture payment
class CheckoutFlowTemplate {
public:
    CheckoutFlowTemplate(OrderService* orders, PaymentService* payments, CaptureService* capture)
        : orders_(orders), payments_(payments), capture_(capture) {}

    RazorpayPayment runCheckout(const CheckoutInput& input) {
        validateInputStep(input);
        const RazorpayOrder order = createOrderStep(input);
        RazorpayPayment payment = initiatePaymentStep(order.orderId, input);
        if (payment.status == PaymentStatus::FAILED) {
            onPaymentFailed(payment);
            return payment;
        }
        payment = capturePaymentStep(payment.paymentId, payment.amountPaise);
        onSuccess(payment);
        return payment;
    }

protected:
    virtual void validateInputStep(const CheckoutInput& input) {
        if (input.amountPaise <= 0) {
            throw std::invalid_argument("checkout amount must be positive");
        }
        if (input.currency.empty()) {
            throw std::invalid_argument("checkout currency required");
        }
        if (input.receipt.empty()) {
            throw std::invalid_argument("checkout receipt required");
        }
        auto validator = PaymentMethodValidatorFactory::create(input.method);
        validator->validate(input.instrumentDetail);
        std::cout << "  [Flow] input validated (" << paymentMethodToString(input.method) << ")\n";
    }

    virtual RazorpayOrder createOrderStep(const CheckoutInput& input) {
        return orders_->createOrder(input.amountPaise, input.currency, input.receipt);
    }

    virtual RazorpayPayment initiatePaymentStep(const std::string& orderId,
                                                const CheckoutInput& input) {
        return payments_->initiatePayment(orderId, input.method, input.instrumentDetail, true);
    }

    virtual RazorpayPayment capturePaymentStep(const std::string& paymentId, int amountPaise) {
        return capture_->capture(paymentId, amountPaise);
    }

    virtual void onSuccess(const RazorpayPayment& payment) {
        std::cout << "  [Flow] checkout complete " << payment.paymentId << " "
                  << paymentStatusToString(payment.status) << "\n";
    }

    virtual void onPaymentFailed(const RazorpayPayment& payment) {
        std::cout << "  [Flow] checkout failed " << payment.failureReason << "\n";
    }

private:
    OrderService* orders_;
    PaymentService* payments_;
    CaptureService* capture_;
};

}  // namespace razorpay_lld

#endif

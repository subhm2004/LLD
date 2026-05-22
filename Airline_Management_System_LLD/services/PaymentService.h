#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_SERVICES_PAYMENTSERVICE_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_SERVICES_PAYMENTSERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>

#include "../enums/PaymentStatus.h"
#include "../models/Payment.h"

namespace airline_mgmt {

class PaymentService {
public:
    static std::string processPayment(const std::string &bookingId, double amount,
                                      std::unordered_map<std::string, Payment> &payments, int &paymentCounter) {
        const std::string paymentId = "PAY_" + std::to_string(++paymentCounter);
        Payment payment{paymentId, bookingId, amount, PaymentStatus::SUCCESS};
        payments.emplace(paymentId, payment);
        return paymentId;
    }

    static void refundPayment(const std::string &bookingId, std::unordered_map<std::string, Payment> &payments) {
        for (auto &entry : payments) {
            if (entry.second.bookingId == bookingId && entry.second.status == PaymentStatus::SUCCESS) {
                entry.second.status = PaymentStatus::REFUNDED;
                return;
            }
        }
        throw std::runtime_error("No successful payment found for booking: " + bookingId);
    }
};

} // namespace airline_mgmt

#endif

// ============================================================================
//  services/PaymentService.h  —  Payment process aur refund (Requirement #2, #6)
// ----------------------------------------------------------------------------
//  Do kaam:
//    1. processPayment() -> booking ka paisa lo, ek Payment record banao (SUCCESS)
//    2. refundPayment()  -> us booking ke SUCCESS payment ko REFUNDED karo
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠ Ye MOCK payment hai — hamesha SUCCESS deta hai                        │
//  │                                                                          │
//  │  Real payment gateway (Razorpay/Stripe) integration OUT OF SCOPE hai.    │
//  │  Yahan focus LLD flow pe hai: payment record banna, booking confirm hona,│
//  │  cancel pe refund hona. Isi liye process hamesha SUCCESS maan leta hai.  │
//  │                                                                          │
//  │  📌 Real system me yahi jagah hoti jahan gateway call jaata, FAILED bhi  │
//  │     ho sakta, retry logic aata. Interview me ye bolna acchha rehta hai   │
//  │     ("main yahan gateway abstract karke Strategy/Adapter laga sakta hoon").│
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Stateless service — payments ka data facade OWN karta hai, ye service
//     us map pe kaam karti hai. `paymentCounter` reference se aata hai taaki
//     unique payment id ("PAY_1", "PAY_2"...) generate ho — counter facade ke
//     paas rehta hai (single source), service usko badha deti hai.
// ============================================================================
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
    // ---- Payment process karo -> SUCCESS record banao, id lauta do ---------
    static std::string processPayment(const std::string &bookingId, double amount,
                                      std::unordered_map<std::string, Payment> &payments, int &paymentCounter) {
        const std::string paymentId = "PAY_" + std::to_string(++paymentCounter);
        Payment payment{paymentId, bookingId, amount, PaymentStatus::SUCCESS}; // mock: hamesha SUCCESS
        payments.emplace(paymentId, payment);
        return paymentId;
    }

    // ---- Booking ka SUCCESS payment dhoondho aur REFUNDED karo -------------
    static void refundPayment(const std::string &bookingId, std::unordered_map<std::string, Payment> &payments) {
        for (auto &entry : payments) {
            // Sirf us booking ka, aur sirf SUCCESS wala payment refund hota hai
            // (pehle se REFUNDED ko dobara refund na kar dein).
            if (entry.second.bookingId == bookingId && entry.second.status == PaymentStatus::SUCCESS) {
                entry.second.status = PaymentStatus::REFUNDED;
                return; // mil gaya, kaam khatam
            }
        }
        // Koi SUCCESS payment nahi mila — kuch gadbad hai (confirmed booking ka
        // payment hona chahiye tha). Saaf error taaki bug turant pakda jaaye.
        throw std::runtime_error("No successful payment found for booking: " + bookingId);
    }
};

} // namespace airline_mgmt

#endif

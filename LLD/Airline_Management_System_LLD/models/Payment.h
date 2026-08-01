// ============================================================================
//  models/Payment.h  —  Ek payment record (ek booking ka paisa)
// ----------------------------------------------------------------------------
//  Har payment kisi ek booking se bandha hota hai (bookingId). Isme kitna paisa
//  (amount) aur uska result (status: SUCCESS/REFUNDED/...) rehta hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ PAYMENT ko BOOKING se ALAG model kyun rakha?                          │
//  │                                                                          │
//  │  Booking = "ticket ki haalat", Payment = "paise ki haalat". Dono ki      │
//  │  apni-apni lifecycle hai. Ek booking cancel hone par uska payment         │
//  │  REFUNDED ho jaata hai — par booking record aur payment record dono       │
//  │  alag rehte hain (audit ke liye — "paisa kab aaya, kab wapas gaya").     │
//  │                                                                          │
//  │  Real system me ek booking ke multiple payment attempts ho sakte hain    │
//  │  (fail hua, phir retry). Alag model rakhne se ye future-proof hai.       │
//  │  📌 Ye Single Responsibility hai: booking apni cheez, payment apni.       │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_PAYMENT_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_PAYMENT_H

#include <string>

#include "../enums/PaymentStatus.h"

namespace airline_mgmt {

struct Payment {
    std::string paymentId; // "PAY_1" — unique
    std::string bookingId; // kis booking ka paisa
    double amount;         // kitna paisa
    PaymentStatus status;  // SUCCESS / REFUNDED / FAILED / PENDING
};

} // namespace airline_mgmt

#endif

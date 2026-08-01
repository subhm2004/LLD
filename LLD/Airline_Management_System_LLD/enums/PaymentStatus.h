// ============================================================================
//  enums/PaymentStatus.h  —  Ek payment ka natija
// ----------------------------------------------------------------------------
//     PENDING  -> shuru hua, abhi result nahi aaya
//     SUCCESS  -> paisa mil gaya (booking confirm ho sakti hai)
//     FAILED   -> payment fail (is demo me use nahi, par real system me hota)
//     REFUNDED -> paisa wapas kar diya (CONFIRMED booking cancel hone par)
//
//  📌 Is demo me PaymentService hamesha SUCCESS deta hai (real gateway out of
//     scope hai). FAILED enum future/real integration ke liye rakha gaya hai —
//     jab asli payment gateway lagega to wo FAILED bhi return kar sakta hai.
//
//  ⚠ Booking ka BookingStatus aur Payment ka PaymentStatus ALAG cheezein hain:
//     - BookingStatus = ticket ki haalat (PENDING/CONFIRMED/...)
//     - PaymentStatus = paise ki haalat (SUCCESS/REFUNDED/...)
//     Ek booking CONFIRMED tab hoti hai jab uska payment SUCCESS ho jaaye.
// ============================================================================
#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_ENUMS_PAYMENTSTATUS_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_ENUMS_PAYMENTSTATUS_H

namespace airline_mgmt {

enum class PaymentStatus { PENDING, SUCCESS, FAILED, REFUNDED };

} // namespace airline_mgmt

#endif

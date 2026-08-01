// ============================================================================
//  services/NotificationService.h  —  Passenger ko khabar do (Requirement #9)
// ----------------------------------------------------------------------------
//  Jab koi ahem event ho (booking confirm, refund, flight change), passenger ko
//  notification bhejo. Is demo me "bhejna" = console pe print karna.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Ye ALAG service kyun? — taaki notification "kaise" bheji jaaye wo     │
//  │     baaki system se DECOUPLED rahe                                        │
//  │                                                                          │
//  │  Abhi console print hai. Kal ko Email / SMS / Push chahiye? Sirf ye class │
//  │  badlegi — booking/payment/facade ka koi code nahi chhuna padega. Ek     │
//  │  hi jagah notification ka logic (Single Responsibility).                  │
//  │                                                                          │
//  │  💡 Aur aage badhaana ho to yahan OBSERVER pattern laga sakte hain        │
//  │     (multiple channels ek saath — email + sms + push). Abhi 3 seedhe     │
//  │     methods kaafi hain (YAGNI — zaroorat se pehle mat banao).            │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Methods `const` hain — ye service kuch store nahi karti, bas message
//     bhejti hai. Passenger ka email facade se aata hai (facade booking se
//     passenger nikaal ke email deta hai). SRP: ye "kise + kya" jaanti hai,
//     "email kahan se laaye" facade ka kaam hai.
// ============================================================================
#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_SERVICES_NOTIFICATIONSERVICE_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_SERVICES_NOTIFICATIONSERVICE_H

#include <iostream>
#include <string>

namespace airline_mgmt {

class NotificationService {
public:
    // Booking confirm hone par (payment success ke baad)
    void sendBookingConfirmed(const std::string &email, const std::string &bookingId, double amount) const {
        std::cout << "[NOTIFY] Booking confirmed -> " << email << " | " << bookingId << " | Rs " << amount << "\n";
    }

    // Refund hone par (CONFIRMED booking cancel hui)
    void sendRefund(const std::string &email, const std::string &bookingId, double amount) const {
        std::cout << "[NOTIFY] Refund processed -> " << email << " | " << bookingId << " | Rs " << amount << "\n";
    }

    // Flight badalne/change hone par
    void sendFlightChange(const std::string &email, const std::string &flightId) const {
        std::cout << "[NOTIFY] Flight schedule change -> " << email << " | flight " << flightId << "\n";
    }
};

} // namespace airline_mgmt

#endif

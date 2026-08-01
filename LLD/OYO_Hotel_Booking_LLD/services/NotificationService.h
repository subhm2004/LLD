// ============================================================================
//  services/NotificationService.h  —  Guest ko khabar do (email/SMS)
// ----------------------------------------------------------------------------
//  Jab koi ahem event ho (booking confirm, cancel, check-in reminder), guest ko
//  notification bhejo. Is demo me "bhejna" = console pe print (email/SMS simulate).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Ye ALAG service kyun? — notification "kaise" bheje wo decouple rahe    │
//  │                                                                          │
//  │  Abhi console print hai. Kal ko asli email/SMS gateway (SendGrid/Twilio)  │
//  │  chahiye? Sirf ye class badlegi — booking/facade ka koi code nahi chhuna  │
//  │  padega. Ek jagah notification logic (Single Responsibility).            │
//  │                                                                          │
//  │  💡 Aage badhaana ho to yahan OBSERVER pattern laga sakte (multiple       │
//  │     channels ek saath: email + SMS + push). Abhi 3 seedhe methods kaafi   │
//  │     hain (YAGNI — zaroorat se pehle mat banao).                          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Methods `const` — ye service kuch store nahi karti, bas message bhejti hai.
//     Guest ka email/phone facade se aata hai (facade booking se guest nikaal ke
//     contact deta). SRP: ye "kise + kya" jaanti, "contact kahan se" facade ka kaam.
// ============================================================================
#ifndef OYO_HOTEL_BOOKING_LLD_SERVICES_NOTIFICATIONSERVICE_H
#define OYO_HOTEL_BOOKING_LLD_SERVICES_NOTIFICATIONSERVICE_H

#include <iostream>
#include <string>

namespace oyo_hotel_lld {

using namespace std;

class NotificationService {
public:
    // Booking confirm hone par (payment success ke baad).
    void sendBookingConfirmed(const string &guestEmail, const string &bookingId, double amount) const {
        cout << "[Email:" << guestEmail << "] Booking " << bookingId << " confirmed. Amount: Rs " << amount << "\n";
    }

    // Booking cancel hone par.
    void sendBookingCancelled(const string &guestEmail, const string &bookingId) const {
        cout << "[Email:" << guestEmail << "] Booking " << bookingId << " cancelled.\n";
    }

    // Check-in reminder (SMS — phone pe).
    void sendCheckInReminder(const string &guestPhone, const string &hotelName, int checkInDay) const {
        cout << "[SMS:" << guestPhone << "] Check-in tomorrow at " << hotelName << " (day " << checkInDay << ")\n";
    }
};

}  // namespace oyo_hotel_lld

#endif

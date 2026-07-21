// ============================================================================
//  models/Booking.h  —  Ek booking record (kaun, kaunsa room, kab, kitne me)
// ----------------------------------------------------------------------------
//  Sab kuch ek jagah jodta hai: guest, hotel, room, check-in/out days, total
//  amount, aur status. Status booking ki poori kahani batata hai (BookingStatus.h).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ DATES = INTEGER "DAYS" (simplified calendar)                          │
//  │                                                                          │
//  │  Real date-time ki jagah simple int days (jaise checkIn=10, checkOut=12). │
//  │  Isse overlap math bahut aasaan: [checkIn, checkOut) half-open interval.  │
//  │  Nights = checkOut - checkIn (12-10 = 2 raat). Interview-scope me int      │
//  │  days perfect; real system me proper Date/LocalDate hota.                │
//  │                                                                          │
//  │  ⭐ HALF-OPEN [checkIn, checkOut): checkOut wale din guest CHALA jaata,   │
//  │  to us din room agle guest ke liye free. Isliye day 10-12 aur day 12-14  │
//  │  wali bookings TOUCH karti hain par OVERLAP nahi (12 shared nahi).       │
//  │  (AvailabilityService::isDateRangeOverlapping me ye logic hai.)          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 `totalAmount_` booking banate waqt PricingService (strategy) se compute
//     hoke aata hai — pehle se calculated store hota (bad me price badle to
//     purani booking ka amount waisa hi rahe).
// ============================================================================
#ifndef OYO_HOTEL_BOOKING_LLD_MODELS_BOOKING_H
#define OYO_HOTEL_BOOKING_LLD_MODELS_BOOKING_H

#include <string>
#include <utility>

#include "../enums/BookingStatus.h"

namespace oyo_hotel_lld {

using namespace std;

class Booking {
public:
    Booking(string bookingId, string guestId, string hotelId, string roomId,
            int checkInDay, int checkOutDay, double totalAmount)
        : bookingId_(std::move(bookingId)),
          guestId_(std::move(guestId)),
          hotelId_(std::move(hotelId)),
          roomId_(std::move(roomId)),
          checkInDay_(checkInDay),
          checkOutDay_(checkOutDay),
          totalAmount_(totalAmount),
          status_(BookingStatus::PENDING) {} // PENDING se shuru (facade CONFIRMED karta)

    const string &getBookingId() const { return bookingId_; }
    const string &getGuestId() const { return guestId_; }
    const string &getHotelId() const { return hotelId_; }
    const string &getRoomId() const { return roomId_; }
    int getCheckInDay() const { return checkInDay_; }
    int getCheckOutDay() const { return checkOutDay_; }
    double getTotalAmount() const { return totalAmount_; }
    BookingStatus getStatus() const { return status_; }

    void setStatus(BookingStatus status) { status_ = status; } // lifecycle transitions

    int getNights() const { return checkOutDay_ - checkInDay_; } // half-open -> raaton ki ginti

private:
    string bookingId_;      // "OYO_1" — unique
    string guestId_;
    string hotelId_;
    string roomId_;
    int checkInDay_;        // simplified calendar (int day)
    int checkOutDay_;       // [checkIn, checkOut) half-open
    double totalAmount_;    // pricing strategy se computed (locked at booking time)
    BookingStatus status_;  // PENDING/CONFIRMED/CHECKED_IN/CHECKED_OUT/CANCELLED
};

}  // namespace oyo_hotel_lld

#endif

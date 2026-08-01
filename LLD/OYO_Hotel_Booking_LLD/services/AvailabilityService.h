// ============================================================================
//  services/AvailabilityService.h  —  Room date-range pe available hai kya (CORE)
// ----------------------------------------------------------------------------
//  Ye service decide karti hai: "ek room diye gaye check-in/out ke liye khaali
//  hai ya nahi?" Yahi hotel booking ka dil hai — ek room alag-alag NON-OVERLAPPING
//  date-ranges pe alag guests ko bik sakta hai (seat/train jaisa segment concept).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ HALF-OPEN INTERVAL OVERLAP — do date-ranges takra rahe hain kya       │
//  │                                                                          │
//  │  A=[inA, outA), B=[inB, outB) overlap  <=>  inA < outB  AND  inB < outA   │
//  │                                                                          │
//  │  Half-open [in, out): checkout wale din guest CHALA jaata, to us din      │
//  │  room agle ke liye free. Isliye:                                          │
//  │     [10,12) vs [12,14): 10<14 haan, 12<12 NAHI -> overlap FALSE ✅ (dono ok)│
//  │     [10,12) vs [11,13): 10<13 haan, 11<12 haan -> overlap TRUE  ❌ (clash) │
//  │                                                                          │
//  │  Closed [in, out] hota to 12 pe checkout aur 12 pe checkin "conflict"     │
//  │  banta (galat). Half-open se ye boundary sahi handle — nazuk detail.     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ isRoomAvailableForStay — availability ka ASLI source (do checks)      │
//  │                                                                          │
//  │  1. Room MAINTENANCE me? -> kabhi available nahi (har date block).        │
//  │  2. Us room ki koi ACTIVE booking is range se overlap? -> not available.  │
//  │     (CANCELLED/CHECKED_OUT bookings SKIP -> wo range ab free.)            │
//  │                                                                          │
//  │  ⚠ Notice: room ka BOOKED/AVAILABLE flag yahan use NAHI hota (sirf        │
//  │  MAINTENANCE) — availability date-overlap se hi decide hoti. (RoomStatus.h│
//  │  ka note dekho — flag decorative hai, ye ledger authority hai.)          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ SAB `static` — stateless service (diya data leke hisaab, khud kuch store nahi).
// ============================================================================
#ifndef OYO_HOTEL_BOOKING_LLD_SERVICES_AVAILABILITYSERVICE_H
#define OYO_HOTEL_BOOKING_LLD_SERVICES_AVAILABILITYSERVICE_H

#include <string>
#include <unordered_map>
#include <vector>

#include "../enums/BookingStatus.h"
#include "../models/Booking.h"
#include "../models/Room.h"

namespace oyo_hotel_lld {

using namespace std;

class AvailabilityService {
public:
    // Do date-ranges overlap karte hain kya (half-open — upar note).
    static bool isDateRangeOverlapping(int checkInA, int checkOutA, int checkInB, int checkOutB) {
        return checkInA < checkOutB && checkInB < checkOutA;
    }

    // ⭐ Ek room is stay ke liye available? (MAINTENANCE + date-overlap check)
    static bool isRoomAvailableForStay(const Room &room, int checkInDay, int checkOutDay,
                                       const unordered_map<string, Booking> &bookings) {
        if (room.getStatus() == RoomStatus::MAINTENANCE) {
            return false; // maintenance = kabhi bookable nahi
        }

        for (const auto &entry : bookings) {
            const Booking &booking = entry.second;
            if (booking.getRoomId() != room.getRoomId()) {
                continue; // doosre room ki booking — skip
            }
            // Cancelled/checked-out bookings ab is range ko block nahi karti.
            if (booking.getStatus() == BookingStatus::CANCELLED ||
                booking.getStatus() == BookingStatus::CHECKED_OUT) {
                continue;
            }
            // Active booking is range se overlap kare -> room busy.
            if (isDateRangeOverlapping(checkInDay, checkOutDay, booking.getCheckInDay(),
                                       booking.getCheckOutDay())) {
                return false;
            }
        }
        return true; // koi clash nahi -> available
    }

    // Diye rooms me se sirf available wale filter karo (search me kaam aata).
    static vector<Room *> filterAvailableRooms(const vector<Room *> &rooms, int checkInDay,
                                               int checkOutDay,
                                               const unordered_map<string, Booking> &bookings) {
        vector<Room *> available;
        for (Room *room : rooms) {
            if (room != nullptr && isRoomAvailableForStay(*room, checkInDay, checkOutDay, bookings)) {
                available.push_back(room);
            }
        }
        return available;
    }
};

}  // namespace oyo_hotel_lld

#endif

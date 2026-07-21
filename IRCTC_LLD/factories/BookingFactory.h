// ============================================================================
//  factories/BookingFactory.h  —  FACTORY PATTERN: TicketBooking banao
// ----------------------------------------------------------------------------
//  Ek CONFIRMED TicketBooking object banane ka saara logic ek jagah. BookingService
//  khud fields nahi bharti — ye factory ko delegate karti hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ FACTORY PATTERN — object creation ko ek jagah centralize karo         │
//  │                                                                          │
//  │  TicketBooking me 10 fields hain (id, user, train, seat, segment...).      │
//  │  Har booking pe ye sab sahi tarah set karna ZAROORI hai. Agar ye kaam      │
//  │  BookingService ke andar hota, aur kal ko koi field add hoti (jaise fare), │
//  │  to har jagah update karna padta. Factory me sab ek jagah -> ek jagah      │
//  │  change. Aur PNR generation (`"PNR" + counter`) bhi yahin encapsulated.   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `static` method — ye ek "pure creation function" hai (input -> booking, koi
//     state nahi). Object banane ki zaroorat nahi: `BookingFactory::createConfirmed(...)`.
//
//  📌 `bookingCounter` reference se aata hai (`int&`) taaki har PNR unique rahe —
//     counter caller (system) ke paas hai (single source), factory usko `++` karke
//     agli id de deti hai. Isi liye pass-by-reference.
// ============================================================================
#ifndef IRCTC_LLD_FACTORIES_BOOKINGFACTORY_H
#define IRCTC_LLD_FACTORIES_BOOKINGFACTORY_H

#include <string>

#include "../enums/BookingStatus.h"
#include "../models/TicketBooking.h"
#include "../models/Train.h"

namespace irctc_lld {

class BookingFactory {
public:
    // Ek fully-filled CONFIRMED booking banao. Saare fields yahan set hote hain.
    static TicketBooking createConfirmed(int& bookingCounter, const std::string& userId,
                                         const Train& train, const std::string& seatId,
                                         const std::string& source, const std::string& destination,
                                         int fromIdx, int toIdx) {
        TicketBooking booking;
        booking.bookingId = "PNR" + std::to_string(++bookingCounter); // unique PNR
        booking.userId = userId;
        booking.trainNumber = train.trainNumber;
        booking.runDate = train.runDate;
        booking.seatId = seatId;
        booking.sourceStation = source;
        booking.destinationStation = destination;
        booking.fromStationIdx = fromIdx; // segment overlap math ke liye
        booking.toStationIdx = toIdx;
        booking.status = BookingStatus::CONFIRMED;
        return booking;
    }
};

}  // namespace irctc_lld

#endif  // IRCTC_LLD_FACTORIES_BOOKINGFACTORY_H

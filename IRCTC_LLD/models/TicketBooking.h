// ============================================================================
//  models/TicketBooking.h  —  Ek booked ticket ka record
// ----------------------------------------------------------------------------
//  Ek booking sab kuch jodti hai: kaunsa user, kaunsi train run, kaunsi seat,
//  kis SEGMENT ke liye (source->destination + unke index), aur status.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `fromStationIdx` / `toStationIdx` — SEGMENT overlap check ka data     │
//  │                                                                          │
//  │  source/destination station CODES hain (BOM, BRC), par overlap check      │
//  │  INDEX pe hota hai (0, 2). Isliye booking me dono store karte hain:        │
//  │  codes (display ke liye) aur index (math ke liye). Ek booking = seat pe    │
//  │  half-open interval [fromStationIdx, toStationIdx).                       │
//  │                                                                          │
//  │  Example: S1 pe Alice BOM->BRC = [0,2), Bob RTM->NDLS = [3,6). Ye overlap  │
//  │  nahi karte, to DONO same seat S1 pe book ho sakte (seat reuse!).         │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 `bookingId` = "PNR1", "PNR2"... (BookingFactory generate karta hai). Cancel
//     isi id se hota hai. `status` CONFIRMED se shuru; cancel pe CANCELLED (record
//     delete nahi hota — bas status badalta, taaki seat segment free ho jaaye).
// ============================================================================
#ifndef IRCTC_LLD_MODELS_TICKETBOOKING_H
#define IRCTC_LLD_MODELS_TICKETBOOKING_H

#include <string>

#include "../enums/BookingStatus.h"

namespace irctc_lld {

struct TicketBooking {
    std::string bookingId;          // "PNR1" — unique PNR
    std::string userId;             // kisne book kiya
    std::string trainNumber;        // kaunsi train
    std::string runDate;            // kis din
    std::string seatId;             // kaunsi physical seat ("S1")
    std::string sourceStation;      // journey start (code, jaise "BOM")
    std::string destinationStation; // journey end (code, jaise "BRC")
    int fromStationIdx{0};          // source ka route-index (overlap math ke liye)
    int toStationIdx{0};            // destination ka route-index
    BookingStatus status{BookingStatus::CONFIRMED}; // CONFIRMED / CANCELLED
};

}  // namespace irctc_lld

#endif  // IRCTC_LLD_MODELS_TICKETBOOKING_H

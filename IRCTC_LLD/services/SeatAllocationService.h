// ============================================================================
//  services/SeatAllocationService.h  —  Segment-based seat allocation ka DIL
// ----------------------------------------------------------------------------
//  Ye service decide karti hai: "kaunsi seat is segment ke liye khaali hai?"
//  Yahi wo jagah hai jahan "ek physical seat, alag-alag non-overlapping segments
//  pe alag passengers" wala IRCTC magic hota hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ CORE RULE: seat is segment ke liye FREE hai agar us seat ki KISI BHI  │
//  │     CONFIRMED booking se ye segment OVERLAP nahi karta                    │
//  │                                                                          │
//  │  isSeatFreeForSegment(seatLedger, [from,to)):                            │
//  │     seatLedger ki har CONFIRMED booking check karo —                     │
//  │     agar koi overlap kare -> seat busy (false)                           │
//  │     koi overlap na kare -> seat free (true)                              │
//  │                                                                          │
//  │  ⭐ CANCELLED bookings ko SKIP karta hai -> cancel hote hi wo segment fir  │
//  │     se free ho jaata hai (booking record delete nahi hota, bas ignore).  │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ SAB METHODS `static` — ye ek STATELESS helper hai (diya gaya data leke
//     hisaab lagati hai, khud kuch store nahi karti). Isi liye akele test karna
//     trivial. (Poore repo me service-layer ka consistent design.)
//
//  📌 `emptyLedger_` — jab kisi seat pe ABHI TAK koi booking nahi hui, to ledger
//     me us seat ki entry hi nahi hoti. Aise me ek shared khaali vector return
//     karte hain (naya banane ke bajaye) — chhoti optimization + saaf code.
// ============================================================================
#ifndef IRCTC_LLD_SERVICES_SEATALLOCATIONSERVICE_H
#define IRCTC_LLD_SERVICES_SEATALLOCATIONSERVICE_H

#include <stdexcept>
#include <string>
#include <vector>

#include "../enums/BookingStatus.h"
#include "../models/Train.h"
#include "../types/SegmentLedger.h"
#include "../utils/SegmentUtils.h"

namespace irctc_lld {

class SeatAllocationService {
public:
    // ⭐ Ek seat is segment [fromIdx, toIdx) ke liye free hai kya?
    static bool isSeatFreeForSegment(const std::vector<TicketBooking>& seatLedger, int fromIdx,
                                     int toIdx) {
        for (const TicketBooking& booking : seatLedger) {
            if (booking.status != BookingStatus::CONFIRMED) {
                continue; // CANCELLED booking ko ignore -> uska segment free
            }
            // Kisi CONFIRMED booking se overlap? -> seat busy is segment ke liye.
            if (segmentsOverlap(fromIdx, toIdx, booking.fromStationIdx, booking.toStationIdx)) {
                return false;
            }
        }
        return true; // kisi se overlap nahi -> free
    }

    // Is segment ke liye kitni seats available hain (count).
    static int countAvailableSeats(const Train& train, const std::vector<std::string>& seats,
                                   const SegmentLedger& ledger, const std::string& source,
                                   const std::string& destination) {
        const int fromIdx = train.stationIndex(source);
        const int toIdx = train.stationIndex(destination);
        if (fromIdx < 0 || toIdx < 0 || toIdx <= fromIdx) {
            throw std::invalid_argument("invalid source/destination for train route");
        }

        int available = 0;
        for (const std::string& seatId : seats) {
            const auto it = ledger.find(seatId);
            // Seat ki koi booking nahi -> khaali ledger use karo (poori free).
            const std::vector<TicketBooking>& seatLedger =
                (it == ledger.end()) ? emptyLedger_ : it->second;
            if (isSeatFreeForSegment(seatLedger, fromIdx, toIdx)) {
                ++available;
            }
        }
        return available;
    }

    // Is segment ke liye available seat IDs ki list.
    static std::vector<std::string> listAvailableSeatIds(const Train& train,
                                                         const std::vector<std::string>& seats,
                                                         const SegmentLedger& ledger,
                                                         const std::string& source,
                                                         const std::string& destination) {
        const int fromIdx = train.stationIndex(source);
        const int toIdx = train.stationIndex(destination);
        if (fromIdx < 0 || toIdx < 0 || toIdx <= fromIdx) {
            throw std::invalid_argument("invalid source/destination for train route");
        }

        std::vector<std::string> available;
        for (const std::string& seatId : seats) {
            const auto it = ledger.find(seatId);
            const std::vector<TicketBooking>& seatLedger =
                (it == ledger.end()) ? emptyLedger_ : it->second;
            if (isSeatFreeForSegment(seatLedger, fromIdx, toIdx)) {
                available.push_back(seatId);
            }
        }
        return available;
    }

    // Auto-allocation: pehli available seat chuno (khaali "" agar koi nahi).
    static std::string pickFirstAvailableSeat(const Train& train, const std::vector<std::string>& seats,
                                              const SegmentLedger& ledger, const std::string& source,
                                              const std::string& destination) {
        const std::vector<std::string> available =
            listAvailableSeatIds(train, seats, ledger, source, destination);
        if (available.empty()) {
            return ""; // koi seat nahi
        }
        return available.front();
    }

private:
    // Jab seat ki koi booking nahi -> ye shared khaali vector (naya banane se bacho).
    inline static const std::vector<TicketBooking> emptyLedger_{};
};

}  // namespace irctc_lld

#endif  // IRCTC_LLD_SERVICES_SEATALLOCATIONSERVICE_H

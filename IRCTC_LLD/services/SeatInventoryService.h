// ============================================================================
//  services/SeatInventoryService.h  —  Har train run ki seats ka list rakho
// ----------------------------------------------------------------------------
//  Simple: ek train run (runKey) ke liye kitni seats hain, unke ids ("S1".."Sn").
//  Ye service sirf seat ke NAAM/list rakhti hai — "kaun kis segment pe booked hai"
//  wo alag (SegmentLedger / BookingService) sambhalta hai (SRP).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ SEATS aur BOOKINGS alag rakhe — kyun?                                 │
//  │                                                                          │
//  │  Ye service: "run X me S1, S2, S3, S4 seats hain" (static list).          │
//  │  BookingService: "S1 pe [0,2) aur [3,6) booked hai" (dynamic ledger).      │
//  │                                                                          │
//  │  Seat ki EXISTENCE (inventory) aur uski BOOKINGS (ledger) do alag concern │
//  │  hain. Alag rakhne se: inventory setup ek baar hota (train add pe), ledger│
//  │  har booking pe badalta. Ek dusre ko chhue bina evolve kar sakte.         │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 `initializeCoach` train add karte waqt (single-threaded setup) call hota hai
//     — "S1".."Sn" naam generate karke store. Booking ke waqt sirf READ hota hai
//     (getSeats) — is service me koi lock nahi chahiye (setup ke baad immutable).
// ============================================================================
#ifndef IRCTC_LLD_SERVICES_SEATINVENTORYSERVICE_H
#define IRCTC_LLD_SERVICES_SEATINVENTORYSERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace irctc_lld {

class SeatInventoryService {
public:
    // Ek run ke liye seatCount seats banao: "S1", "S2", ... "Sn".
    void initializeCoach(const std::string& runKey, int seatCount) {
        if (seatCount < 1) {
            throw std::invalid_argument("seatCount must be >= 1");
        }
        std::vector<std::string> seats;
        seats.reserve(static_cast<size_t>(seatCount));
        for (int i = 1; i <= seatCount; ++i) {
            seats.push_back("S" + std::to_string(i));
        }
        seatsByRun_[runKey] = std::move(seats);
    }

    // Ek run ki saari seat ids. Nahi mila -> error (run register nahi hua).
    const std::vector<std::string>& getSeats(const std::string& runKey) const {
        auto it = seatsByRun_.find(runKey);
        if (it == seatsByRun_.end()) {
            throw std::runtime_error("seat inventory not found for run: " + runKey);
        }
        return it->second;
    }

private:
    // runKey -> uski seat ids ("S1".."Sn"). Setup ke baad immutable (read-only).
    std::unordered_map<std::string, std::vector<std::string>> seatsByRun_;
};

}  // namespace irctc_lld

#endif  // IRCTC_LLD_SERVICES_SEATINVENTORYSERVICE_H

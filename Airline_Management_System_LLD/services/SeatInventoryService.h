// ============================================================================
//  services/SeatInventoryService.h  —  Flight ki seats manage karo
// ----------------------------------------------------------------------------
//  Iska kaam: jab aircraft flight ko assign ho, to us flight ki asli seats
//  BANAO (aircraft ki capacity ke hisaab se). Aur available seats nikaalna,
//  ek seat dhoondhna — sab yahi karti hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ SEATS FLIGHT ke saath banti hain, AIRCRAFT ke saath nahi             │
//  │                                                                          │
//  │  Aircraft sirf "template" hai (30 economy + 6 premium + 4 business).      │
//  │  Jab wo aircraft kisi flight ko assign hota hai, tab us FLIGHT ke liye    │
//  │  utni seats generate hoti hain — har seat ka id flightId se juda hota     │
//  │  ("AI-101-E1"). Ek hi aircraft do flights pe lage to dono ki apni-apni    │
//  │  seats banti hain (alag ids). (Aircraft.h me poora note hai.)            │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `generateSeatsForFlight` pehle `clearSeatsForFlight` bulata hai — kyun?
//     Taaki agar us flight pe DOBARA aircraft assign ho (aircraft badla), to
//     purani seats pehle saaf ho jaayein. Warna purani + nayi seats mix ho
//     jaatin (ghost seats). "Pehle saaf, phir banao" = idempotent operation.
//
//  📌 Pricing: yahan sirf BASE price set hota hai (Economy 3500, Premium 5500,
//     Business 12000). Final fare booking ke waqt PricingStrategy nikaalti hai
//     (base ko multiply karke). Seat generation aur pricing alag concerns hain.
// ============================================================================
#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_SERVICES_SEATINVENTORYSERVICE_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_SERVICES_SEATINVENTORYSERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../enums/SeatClass.h"
#include "../enums/SeatStatus.h"
#include "../models/Aircraft.h"
#include "../models/Seat.h"

namespace airline_mgmt {

class SeatInventoryService {
public:
    // ---- Ek flight ki saari seats hata do (re-assign se pehle safai) --------
    static void clearSeatsForFlight(const std::string &flightId, std::unordered_map<std::string, Seat> &seats) {
        // erase-while-iterate: `erase` naya valid iterator lauta ta hai, usi ko
        // aage badhaate hain. Agar `++it` galti se erase ke baad karte to
        // invalid iterator pe crash hota — isi liye ye careful pattern.
        for (auto it = seats.begin(); it != seats.end();) {
            if (it->second.flightId == flightId) {
                it = seats.erase(it); // erase lauta ta hai next valid iterator
            } else {
                ++it;
            }
        }
    }

    // ---- Aircraft capacity ke hisaab se flight ki seats BANAO ---------------
    static void generateSeatsForFlight(const Aircraft &aircraft, const std::string &flightId,
                                       std::unordered_map<std::string, Seat> &seats) {
        clearSeatsForFlight(flightId, seats); // pehle purani seats saaf (idempotent)
        int row = 1;
        // Economy seats — "E1", "E2", ... base price 3500
        for (int i = 0; i < aircraft.economySeats; ++i) {
            const int seatRow = row++;
            addSeat(seats, flightId, "E" + std::to_string(seatRow), seatRow, 1, SeatClass::ECONOMY, 3500.0);
        }
        // Premium economy — "P...", base price 5500
        for (int i = 0; i < aircraft.premiumEconomySeats; ++i) {
            const int seatRow = row++;
            addSeat(seats, flightId, "P" + std::to_string(seatRow), seatRow, 1, SeatClass::PREMIUM_ECONOMY, 5500.0);
        }
        // Business — "B...", base price 12000
        for (int i = 0; i < aircraft.businessSeats; ++i) {
            const int seatRow = row++;
            addSeat(seats, flightId, "B" + std::to_string(seatRow), seatRow, 1, SeatClass::BUSINESS, 12000.0);
        }
    }

    // ---- Kisi flight ki khaali (AVAILABLE) seats laao -----------------------
    static std::vector<const Seat *> getAvailableSeats(const std::string &flightId,
                                                       const std::unordered_map<std::string, Seat> &seats) {
        std::vector<const Seat *> result;
        for (const auto &entry : seats) {
            if (entry.second.flightId == flightId && entry.second.status == SeatStatus::AVAILABLE) {
                result.push_back(&entry.second);
            }
        }
        return result;
    }

    // ---- Ek seat dhoondho (non-const ref taaki uska status badla ja sake) ---
    // Nahi mili to throw — caller ko galat seat id ka saaf pata chal jaata hai.
    static Seat &getSeatOrThrow(const std::string &seatId, std::unordered_map<std::string, Seat> &seats) {
        auto it = seats.find(seatId);
        if (it == seats.end()) {
            throw std::runtime_error("Seat not found: " + seatId);
        }
        return it->second;
    }

private:
    // Ek seat map me daalo. Id pehle se ho to duplicate error (safety check).
    static void addSeat(std::unordered_map<std::string, Seat> &seats, const std::string &flightId,
                        const std::string &suffix, int row, int col, SeatClass seatClass, double basePrice) {
        const std::string seatId = flightId + "-" + suffix; // "AI-101-E1"
        if (seats.count(seatId)) {
            throw std::runtime_error("Duplicate seat id: " + seatId);
        }
        seats.emplace(seatId, Seat{seatId, flightId, row, col, seatClass, SeatStatus::AVAILABLE, basePrice});
    }
};

} // namespace airline_mgmt

#endif

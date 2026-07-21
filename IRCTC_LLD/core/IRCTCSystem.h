// ============================================================================
//  core/IRCTCSystem.h  —  Poore system ka FACADE (single entry point)
// ----------------------------------------------------------------------------
//  Ye class hi client (main.cpp) ke liye "IRCTC" hai. Andar 5 services + factory
//  ka poora tamasha chalta hai, par bahar se client ko sirf saaf methods dikhte
//  hain: registerUser, addTrainRun, searchByRoute, bookTicket, cancelTicket...
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ FACADE PATTERN — subsystem ki complexity client se chhupao           │
//  │                                                                          │
//  │  Client ko ye NAHI pata:                                                 │
//  │    - trains kaise store/search hoti  (Catalog + Search services)         │
//  │    - seats kaise allot hoti          (SeatInventory + SeatAllocation)    │
//  │    - segment overlap kaise check     (SegmentUtils)                      │
//  │    - concurrency kaise handle        (BookingService per-run mutex)      │
//  │  Wo bas facade ke method bulata hai; facade sahi service ko delegate.    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ FACADE khud data ka MAALIK hai (catalog, inventory, bookingService, users)
//     aur unhe wire karta hai (`bookingService_{&inventory_}` — booking ko
//     inventory ka reference milta seats ke liye). Services aapas me directly
//     baat nahi karti — facade orchestrate karta hai.
//
//  📌 `addTrainRun` DO kaam karta hai (aur ek fix-critical teesra):
//     1. catalog me train add
//     2. inventory me seats banao
//     3. ⭐ bookingService_.registerRun() — per-run mutex+ledger PRE-CREATE karo
//        (SETUP-time, single-threaded). Ye concurrency-fix ka hissa hai: taaki
//        baad me concurrent booking ke waqt shared maps me insert na ho (race).
//        (BookingService.h me poora fix note padho.)
// ============================================================================
#ifndef IRCTC_LLD_CORE_IRCTCSYSTEM_H
#define IRCTC_LLD_CORE_IRCTCSYSTEM_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../models/TicketBooking.h"
#include "../models/Train.h"
#include "../models/User.h"
#include "../services/BookingService.h"
#include "../services/SeatAllocationService.h"
#include "../services/SeatInventoryService.h"
#include "../services/TrainCatalogService.h"
#include "../services/TrainSearchService.h"

namespace irctc_lld {

class IRCTCSystem {
public:
    // ---- User register (unique id generate) --------------------------------
    std::string registerUser(const std::string& name) {
        if (name.empty()) {
            throw std::invalid_argument("name required");
        }
        const std::string userId = "USR_" + std::to_string(++userCounter_);
        users_[userId] = User{userId, name};
        return userId;
    }

    // ---- Train run add: catalog + inventory + ⭐ per-run state pre-create ----
    void addTrainRun(const Train& train, int coachSeatCount) {
        catalog_.addTrain(train);                              // 1. catalog me
        inventory_.initializeCoach(train.runKey(), coachSeatCount); // 2. seats
        bookingService_.registerRun(train.runKey());           // 3. ⭐ mutex+ledger pre-create (fix)
    }

    // ---- FR-1: route + date se search --------------------------------------
    std::vector<const Train*> searchByRoute(const std::string& source, const std::string& destination,
                                            const std::string& date) const {
        return TrainSearchService::searchByRoute(catalog_, source, destination, date);
    }

    // ---- FR-2: train number + date se search -------------------------------
    const Train* searchByTrainNumber(const std::string& trainNumber, const std::string& date) const {
        return TrainSearchService::searchByTrainNumber(catalog_, trainNumber, date);
    }

    // ---- FR-3: is segment ke liye available seat count ---------------------
    int getAvailableSeatCount(const std::string& trainNumber, const std::string& date,
                              const std::string& source, const std::string& destination) const {
        const Train& train = catalog_.getTrainRun(trainNumber, date);
        const std::string runKey = train.runKey();
        return SeatAllocationService::countAvailableSeats(
            train, inventory_.getSeats(runKey), bookingService_.getLedgerForRun(runKey), source,
            destination);
    }

    std::vector<std::string> listAvailableSeats(const std::string& trainNumber,
                                                const std::string& date, const std::string& source,
                                                const std::string& destination) const {
        const Train& train = catalog_.getTrainRun(trainNumber, date);
        const std::string runKey = train.runKey();
        return SeatAllocationService::listAvailableSeatIds(
            train, inventory_.getSeats(runKey), bookingService_.getLedgerForRun(runKey), source,
            destination);
    }

    // ---- FR-4: book ticket (auto ya preferred seat) ------------------------
    TicketBooking bookTicket(const std::string& userId, const std::string& trainNumber,
                             const std::string& date, const std::string& source,
                             const std::string& destination,
                             const std::string& preferredSeatId = "") {
        validateUser(userId);
        const Train& train = catalog_.getTrainRun(trainNumber, date);
        return bookingService_.bookTicket(train, userId, source, destination, preferredSeatId,
                                          bookingCounter_);
    }

    // ---- FR-7: cancel ------------------------------------------------------
    void cancelTicket(const std::string& bookingId, const std::string& userId) {
        validateUser(userId);
        bookingService_.cancelTicket(bookingId, userId);
    }

    // ⭐ BY VALUE (const ref nahi) — BookingService thread-safety ke liye copy
    //    lauta ta hai; agar hum yahan const ref lauta te to wo temporary ka
    //    dangling reference ban jaata. main.cpp `const TicketBooking& = ...` se
    //    bind karta hai -> temporary ki lifetime extend ho jaati (safe).
    TicketBooking getBooking(const std::string& bookingId) const {
        return bookingService_.getBooking(bookingId);
    }

private:
    // ---- Services (facade owns + wires them) -------------------------------
    TrainCatalogService catalog_;
    SeatInventoryService inventory_;
    BookingService bookingService_{&inventory_}; // booking ko inventory ka ref milta

    std::unordered_map<std::string, User> users_;
    int userCounter_{0};
    int bookingCounter_{0}; // PNR ke liye (BookingFactory isse ++ karta)

    void validateUser(const std::string& userId) const {
        if (users_.find(userId) == users_.end()) {
            throw std::runtime_error("user not found: " + userId);
        }
    }
};

}  // namespace irctc_lld

#endif  // IRCTC_LLD_CORE_IRCTCSYSTEM_H

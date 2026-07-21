// ============================================================================
//  services/BookingService.h  —  Booking + cancel + CONCURRENCY (NFR-2)
// ----------------------------------------------------------------------------
//  Ye service asli booking karti hai: seat chuno (ya validate karo), CONFIRMED
//  booking banao (Factory se), ledger me daalo, index me daalo. Aur concurrent
//  bookings ko safely handle karti hai (per-train-run mutex).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ PER-TRAIN-RUN MUTEX — "ek run ek lock" (lock striping ki tarah)       │
//  │                                                                          │
//  │  Har train run (runKey) ka apna mutex. Ek run ki bookings serialize hoti  │
//  │  (ek waqt me ek), par ALAG runs PARALLEL book ho sakte (alag locks). Isse │
//  │  "1 seat, 8 log" wali race me sirf 1 jeetta hai (baaki ko seat busy       │
//  │  milti) — bina poore system ko ek lock se atkaaye.                        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  🐛 CONCURRENCY BUG JO THA (ThreadSanitizer ne pakda) + FIX             │
//  │                                                                          │
//  │  PROBLEM (chicken-and-egg): per-run mutex ek MAP me store tha             │
//  │  (`runMutexes_`). Us mutex ko NIKAALNE ke liye `runMutexes_[runKey]`      │
//  │  karte the — par ye operator[] khud MAP KO MODIFY (insert) kar sakta hai! │
//  │  8 threads ek saath `runMutexes_[runKey]` -> hash-table structure pe DATA │
//  │  RACE (lock lene se PEHLE hi race, kyunki lock to isi map me hai). 💥     │
//  │                                                                          │
//  │  ✅ FIX (do hisse):                                                       │
//  │   1. PRE-CREATE per-run state (`registerRun`) SETUP time pe (single-      │
//  │      threaded, jab train add hoti). Ab booking ke waqt runMutexes_ aur    │
//  │      ledgerByRun_ me koi INSERT nahi — sirf `.at()` LOOKUP (concurrent    │
//  │      const reads safe hote hain).                                        │
//  │   2. Global `bookingsById_` index ko alag `bookingsMutex_` se guard karo. │
//  │                                                                          │
//  │  ⚠ DEADLOCK avoid: run-lock aur bookings-lock kabhi EK SAATH nahi pakde   │
//  │     jaate. bookTicket: pehle run-lock (ledger), phir alag se bookings-    │
//  │     lock (index). cancel: pehle bookings-lock (index), phir alag se run-  │
//  │     lock (ledger). Sequential (nested nahi) -> koi lock-ordering cycle nahi│
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef IRCTC_LLD_SERVICES_BOOKINGSERVICE_H
#define IRCTC_LLD_SERVICES_BOOKINGSERVICE_H

#include <mutex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../enums/BookingStatus.h"
#include "../factories/BookingFactory.h"
#include "../models/TicketBooking.h"
#include "../models/Train.h"
#include "../types/SegmentLedger.h"
#include "SeatAllocationService.h"
#include "SeatInventoryService.h"

namespace irctc_lld {

class BookingService {
public:
    explicit BookingService(SeatInventoryService* inventory) : inventory_(inventory) {}

    // ⭐ SETUP-TIME pre-create per-run state (mutex + ledger). Single-threaded
    // (train add hote waqt) — isi liye yahan operator[] insert safe hai. Iske
    // baad booking me sirf .at() lookup hota -> concurrent access safe.
    void registerRun(const std::string& runKey) {
        std::lock_guard<std::mutex> guard(registryMutex_);
        runMutexes_[runKey];   // per-run mutex bana do (default-constructed)
        ledgerByRun_[runKey];  // us run ka khaali ledger bana do
    }

    // ========================================================================
    //  BOOK TICKET — seat chuno/validate karo + CONFIRMED booking banao
    // ========================================================================
    TicketBooking bookTicket(const Train& train, const std::string& userId, const std::string& source,
                             const std::string& destination, const std::string& preferredSeatId,
                             int& bookingCounter) {
        const std::string runKey = train.runKey();

        const int fromIdx = train.stationIndex(source);
        const int toIdx = train.stationIndex(destination);
        if (fromIdx < 0 || toIdx < 0 || toIdx <= fromIdx) {
            throw std::invalid_argument("invalid journey segment on this train");
        }

        const std::vector<std::string>& seats = inventory_->getSeats(runKey);

        TicketBooking booking;
        // ---- PHASE 1: ledger work UNDER RUN LOCK (per-run serialize) ---------
        {
            std::lock_guard<std::mutex> guard(mutexForRun(runKey)); // .at() -> safe lookup
            SegmentLedger& ledger = ledgerForRun(runKey);           // pre-created entry (.at)

            std::string seatId = preferredSeatId;
            if (seatId.empty()) {
                // Auto: pehli available seat is segment ke liye
                seatId = SeatAllocationService::pickFirstAvailableSeat(train, seats, ledger, source,
                                                                       destination);
            } else {
                // Preferred: wo seat is segment ke liye free hai ya nahi
                const auto it = ledger.find(seatId);
                const std::vector<TicketBooking>& seatLedger =
                    (it == ledger.end()) ? emptyLedger_ : it->second;
                if (!SeatAllocationService::isSeatFreeForSegment(seatLedger, fromIdx, toIdx)) {
                    throw std::runtime_error("seat not available for segment: " + seatId);
                }
            }

            if (seatId.empty()) {
                throw std::runtime_error("no seats available for requested segment");
            }

            booking = BookingFactory::createConfirmed(bookingCounter, userId, train, seatId, source,
                                                      destination, fromIdx, toIdx);
            ledger[seatId].push_back(booking); // seat pe ye segment reserve (run lock me safe)
        } // <- run lock RELEASE (bookings lock ke saath nesting nahi)

        // ---- PHASE 2: global index UNDER BOOKINGS LOCK ----------------------
        {
            std::lock_guard<std::mutex> guard(bookingsMutex_);
            bookingsById_[booking.bookingId] = booking; // PNR se lookup ke liye
        }
        return booking;
    }

    // ========================================================================
    //  CANCEL TICKET — status CANCELLED (seat segment fir se free)
    // ========================================================================
    void cancelTicket(const std::string& bookingId, const std::string& userId) {
        std::string runKey;
        std::string seatId;

        // ---- PHASE 1: index me find + validate + mark, phir lock chhodo -----
        {
            std::lock_guard<std::mutex> guard(bookingsMutex_);
            auto it = bookingsById_.find(bookingId);
            if (it == bookingsById_.end()) {
                throw std::runtime_error("booking not found: " + bookingId);
            }
            TicketBooking& booking = it->second;
            if (booking.userId != userId) {
                throw std::runtime_error("user not allowed to cancel booking: " + bookingId);
            }
            if (booking.status == BookingStatus::CANCELLED) {
                throw std::runtime_error("booking already cancelled: " + bookingId);
            }
            booking.status = BookingStatus::CANCELLED; // index me mark
            runKey = booking.trainNumber + "|" + booking.runDate; // ledger update ke liye copy
            seatId = booking.seatId;
        } // <- bookings lock RELEASE (run lock ke saath nesting nahi)

        // ---- PHASE 2: ledger me bhi wahi booking CANCELLED (run lock) --------
        {
            std::lock_guard<std::mutex> guard(mutexForRun(runKey));
            SegmentLedger& ledger = ledgerForRun(runKey);
            for (TicketBooking& segment : ledger[seatId]) {
                if (segment.bookingId == bookingId) {
                    segment.status = BookingStatus::CANCELLED; // ab ye segment SeatAllocation me free ganega
                }
            }
        }
    }

    // ⭐ BY VALUE return (const ref nahi) — thread-safety ke liye: lock chhodne
    //    ke baad map-element ka reference invalid ho sakta (koi insert kare), to
    //    ek COPY lauta dena safe hai.
    TicketBooking getBooking(const std::string& bookingId) const {
        std::lock_guard<std::mutex> guard(bookingsMutex_);
        auto it = bookingsById_.find(bookingId);
        if (it == bookingsById_.end()) {
            throw std::runtime_error("booking not found: " + bookingId);
        }
        return it->second; // copy
    }

    // Availability queries ke liye ledger read (demo me single-threaded call hota).
    const SegmentLedger& getLedgerForRun(const std::string& runKey) const {
        static const SegmentLedger empty{};
        auto it = ledgerByRun_.find(runKey);
        if (it == ledgerByRun_.end()) {
            return empty;
        }
        return it->second;
    }

private:
    SeatInventoryService* inventory_;                              // seats ka source (owned by system)
    std::unordered_map<std::string, TicketBooking> bookingsById_;  // PNR -> booking (global index)
    std::unordered_map<std::string, SegmentLedger> ledgerByRun_;   // runKey -> seat/segment ledger
    std::unordered_map<std::string, std::mutex> runMutexes_;       // runKey -> per-run lock

    mutable std::mutex registryMutex_;  // runMutexes_/ledgerByRun_ ke STRUCTURE ko guard (setup)
    mutable std::mutex bookingsMutex_;  // bookingsById_ index ko guard (cross-run safe)

    inline static const std::vector<TicketBooking> emptyLedger_{};

    // ⭐ Ab .at() (lookup, insert nahi) — entries registerRun me pre-created hain,
    //    isliye concurrent const lookups safe. (Pehle [] insert karta tha -> race.)
    std::mutex& mutexForRun(const std::string& runKey) { return runMutexes_.at(runKey); }
    SegmentLedger& ledgerForRun(const std::string& runKey) { return ledgerByRun_.at(runKey); }
};

}  // namespace irctc_lld

#endif  // IRCTC_LLD_SERVICES_BOOKINGSERVICE_H

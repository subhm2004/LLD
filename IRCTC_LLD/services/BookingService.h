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

    TicketBooking bookTicket(const Train& train, const std::string& userId, const std::string& source,
                             const std::string& destination, const std::string& preferredSeatId,
                             int& bookingCounter) {
        const std::string runKey = train.runKey();
        std::mutex& runLock = mutexForRun(runKey);
        std::lock_guard<std::mutex> guard(runLock);

        const int fromIdx = train.stationIndex(source);
        const int toIdx = train.stationIndex(destination);
        if (fromIdx < 0 || toIdx < 0 || toIdx <= fromIdx) {
            throw std::invalid_argument("invalid journey segment on this train");
        }

        const std::vector<std::string>& seats = inventory_->getSeats(runKey);
        SegmentLedger& ledger = ledgerByRun_[runKey];

        std::string seatId = preferredSeatId;
        if (seatId.empty()) {
            seatId = SeatAllocationService::pickFirstAvailableSeat(train, seats, ledger, source,
                                                                   destination);
        } else {
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

        TicketBooking booking = BookingFactory::createConfirmed(
            bookingCounter, userId, train, seatId, source, destination, fromIdx, toIdx);
        ledger[seatId].push_back(booking);
        bookingsById_[booking.bookingId] = booking;
        return booking;
    }

    void cancelTicket(const std::string& bookingId, const std::string& userId) {
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

        const std::string runKey = booking.trainNumber + "|" + booking.runDate;
        std::mutex& runLock = mutexForRun(runKey);
        std::lock_guard<std::mutex> guard(runLock);

        booking.status = BookingStatus::CANCELLED;
        SegmentLedger& ledger = ledgerByRun_[runKey];
        for (TicketBooking& segment : ledger[booking.seatId]) {
            if (segment.bookingId == bookingId) {
                segment.status = BookingStatus::CANCELLED;
            }
        }
    }

    const TicketBooking& getBooking(const std::string& bookingId) const {
        auto it = bookingsById_.find(bookingId);
        if (it == bookingsById_.end()) {
            throw std::runtime_error("booking not found: " + bookingId);
        }
        return it->second;
    }

    const SegmentLedger& getLedgerForRun(const std::string& runKey) const {
        static const SegmentLedger empty{};
        auto it = ledgerByRun_.find(runKey);
        if (it == ledgerByRun_.end()) {
            return empty;
        }
        return it->second;
    }

    SegmentLedger& getLedgerForRun(const std::string& runKey) { return ledgerByRun_[runKey]; }

private:
    SeatInventoryService* inventory_;
    std::unordered_map<std::string, TicketBooking> bookingsById_;
    std::unordered_map<std::string, SegmentLedger> ledgerByRun_;
    std::unordered_map<std::string, std::mutex> runMutexes_;

    inline static const std::vector<TicketBooking> emptyLedger_{};

    std::mutex& mutexForRun(const std::string& runKey) { return runMutexes_[runKey]; }
};

}  // namespace irctc_lld

#endif  // IRCTC_LLD_SERVICES_BOOKINGSERVICE_H

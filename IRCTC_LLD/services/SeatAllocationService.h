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
    static bool isSeatFreeForSegment(const std::vector<TicketBooking>& seatLedger, int fromIdx,
                                     int toIdx) {
        for (const TicketBooking& booking : seatLedger) {
            if (booking.status != BookingStatus::CONFIRMED) {
                continue;
            }
            if (segmentsOverlap(fromIdx, toIdx, booking.fromStationIdx, booking.toStationIdx)) {
                return false;
            }
        }
        return true;
    }

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
            const std::vector<TicketBooking>& seatLedger =
                (it == ledger.end()) ? emptyLedger_ : it->second;
            if (isSeatFreeForSegment(seatLedger, fromIdx, toIdx)) {
                ++available;
            }
        }
        return available;
    }

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

    static std::string pickFirstAvailableSeat(const Train& train, const std::vector<std::string>& seats,
                                              const SegmentLedger& ledger, const std::string& source,
                                              const std::string& destination) {
        const std::vector<std::string> available =
            listAvailableSeatIds(train, seats, ledger, source, destination);
        if (available.empty()) {
            return "";
        }
        return available.front();
    }

private:
    inline static const std::vector<TicketBooking> emptyLedger_{};
};

}  // namespace irctc_lld

#endif  // IRCTC_LLD_SERVICES_SEATALLOCATIONSERVICE_H

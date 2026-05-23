#ifndef IRCTC_LLD_TYPES_SEGMENTLEDGER_H
#define IRCTC_LLD_TYPES_SEGMENTLEDGER_H

#include <string>
#include <unordered_map>
#include <vector>

#include "../models/TicketBooking.h"

namespace irctc_lld {

/**
 * Per train-run ledger: seat id -> segment bookings on that seat.
 * Extends map so existing ledger[key] / find() code stays unchanged.
 */
class SegmentLedger : public std::unordered_map<std::string, std::vector<TicketBooking>> {
public:
    using Map = std::unordered_map<std::string, std::vector<TicketBooking>>;
    using Map::Map;
};

}  // namespace irctc_lld

#endif  // IRCTC_LLD_TYPES_SEGMENTLEDGER_H

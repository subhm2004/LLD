#ifndef MEETING_SCHEDULER_LLD_STRATEGIES_IFREESLOTSTRATEGY_H
#define MEETING_SCHEDULER_LLD_STRATEGIES_IFREESLOTSTRATEGY_H

#include <string>
#include <unordered_map>
#include <vector>

#include "../models/AvailabilityWindow.h"
#include "../models/Meeting.h"
#include "../models/TimeSlot.h"

namespace meeting_scheduler_lld {

class IFreeSlotStrategy {
public:
    virtual ~IFreeSlotStrategy() = default;

    virtual std::vector<TimeSlot> findSlots(
        const std::string& date, int durationMinutes,
        const std::vector<std::string>& userIds,
        const std::unordered_map<std::string, std::vector<AvailabilityWindow>>& availability,
        const std::unordered_map<std::string, Meeting>& meetings) const = 0;
};

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_STRATEGIES_IFREESLOTSTRATEGY_H

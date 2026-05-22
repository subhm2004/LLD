#ifndef MEETING_SCHEDULER_LLD_SERVICES_SLOTFINDERSERVICE_H
#define MEETING_SCHEDULER_LLD_SERVICES_SLOTFINDERSERVICE_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../models/AvailabilityWindow.h"
#include "../models/Meeting.h"
#include "../models/TimeSlot.h"
#include "../strategies/EarliestMutualSlotStrategy.h"
#include "../strategies/IFreeSlotStrategy.h"

namespace meeting_scheduler_lld {

class SlotFinderService {
public:
    SlotFinderService() : strategy_(std::make_shared<EarliestMutualSlotStrategy>()) {}

    explicit SlotFinderService(std::shared_ptr<IFreeSlotStrategy> strategy)
        : strategy_(std::move(strategy)) {}

    std::vector<TimeSlot> findMutualSlots(
        const std::vector<std::string>& userIds, const std::string& date, int durationMinutes,
        const std::unordered_map<std::string, std::vector<AvailabilityWindow>>& availability,
        const std::unordered_map<std::string, Meeting>& meetings) const {
        return strategy_->findSlots(date, durationMinutes, userIds, availability, meetings);
    }

private:
    std::shared_ptr<IFreeSlotStrategy> strategy_;
};

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_SERVICES_SLOTFINDERSERVICE_H

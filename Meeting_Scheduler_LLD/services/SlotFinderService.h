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

/**
 * @class SlotFinderService
 * @brief Strategy pattern ko utilize karke multiple users ke mutual free slots dhoondhne wali service.
 * 
 * Client code dynamic strategy inject kar sakta hai. By default, ye `EarliestMutualSlotStrategy`
 * use karti hai jo common free time block identify karne me help karti hai.
 */
class SlotFinderService {
public:
    // Default constructor jo default strategy allocate karta hai.
    SlotFinderService() : strategy_(std::make_shared<EarliestMutualSlotStrategy>()) {}

    // Constructor agar dynamic strategy inject karni ho.
    explicit SlotFinderService(std::shared_ptr<IFreeSlotStrategy> strategy)
        : strategy_(std::move(strategy)) {}

    // Diye gaye list of users ke liye mutual available free slots calculate karne ka function.
    std::vector<TimeSlot> findMutualSlots(
        const std::vector<std::string>& userIds, const std::string& date, int durationMinutes,
        const std::unordered_map<std::string, std::vector<AvailabilityWindow>>& availability,
        const std::unordered_map<std::string, Meeting>& meetings) const {
        return strategy_->findSlots(date, durationMinutes, userIds, availability, meetings);
    }

private:
    std::shared_ptr<IFreeSlotStrategy> strategy_; // Inject ki gayi interface strategy pointer (Strategy Pattern)
};

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_SERVICES_SLOTFINDERSERVICE_H

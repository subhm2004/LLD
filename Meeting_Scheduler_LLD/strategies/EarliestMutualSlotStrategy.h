#ifndef MEETING_SCHEDULER_LLD_STRATEGIES_EARLIESTMUTUALSLOTSTRATEGY_H
#define MEETING_SCHEDULER_LLD_STRATEGIES_EARLIESTMUTUALSLOTSTRATEGY_H

#include <algorithm>
#include <stdexcept>
#include <vector>

#include "IFreeSlotStrategy.h"

namespace meeting_scheduler_lld {

class EarliestMutualSlotStrategy : public IFreeSlotStrategy {
public:
    std::vector<TimeSlot> findSlots(
        const std::string& date, int durationMinutes,
        const std::vector<std::string>& userIds,
        const std::unordered_map<std::string, std::vector<AvailabilityWindow>>& availability,
        const std::unordered_map<std::string, Meeting>& meetings) const override {
        if (durationMinutes <= 0) {
            throw std::invalid_argument("duration must be positive");
        }
        if (userIds.empty()) {
            return {};
        }

        int dayStart = 24 * 60;
        int dayEnd = 0;
        bool hasAvailability = false;

        for (const std::string& userId : userIds) {
            auto it = availability.find(userId);
            if (it == availability.end()) {
                continue;
            }
            for (const AvailabilityWindow& window : it->second) {
                if (window.getDate() != date) {
                    continue;
                }
                hasAvailability = true;
                dayStart = std::min(dayStart, window.getStartMinutes());
                dayEnd = std::max(dayEnd, window.getEndMinutes());
            }
        }

        if (!hasAvailability) {
            return {};
        }

        std::vector<TimeSlot> slots;
        const int step = 15;

        for (int start = dayStart; start + durationMinutes <= dayEnd; start += step) {
            const int end = start + durationMinutes;
            if (isSlotFree(date, start, end, userIds, availability, meetings)) {
                slots.emplace_back(date, start, end);
            }
        }
        return slots;
    }

private:
    bool isSlotFree(
        const std::string& date, int start, int end, const std::vector<std::string>& userIds,
        const std::unordered_map<std::string, std::vector<AvailabilityWindow>>& availability,
        const std::unordered_map<std::string, Meeting>& meetings) const {
        for (const std::string& userId : userIds) {
            if (!isWithinAvailability(userId, date, start, end, availability)) {
                return false;
            }
            if (hasMeetingConflict(userId, date, start, end, meetings)) {
                return false;
            }
        }
        return true;
    }

    bool isWithinAvailability(
        const std::string& userId, const std::string& date, int start, int end,
        const std::unordered_map<std::string, std::vector<AvailabilityWindow>>& availability)
        const {
        auto it = availability.find(userId);
        if (it == availability.end()) {
            return false;
        }
        for (const AvailabilityWindow& window : it->second) {
            if (window.getDate() == date && window.contains(start, end)) {
                return true;
            }
        }
        return false;
    }

    bool hasMeetingConflict(const std::string& userId, const std::string& date, int start,
                            int end,
                            const std::unordered_map<std::string, Meeting>& meetings) const {
        for (const auto& entry : meetings) {
            const Meeting& meeting = entry.second;
            if (!meeting.isActive() || meeting.getDate() != date) {
                continue;
            }
            if (meeting.involvesUser(userId) && meeting.overlaps(start, end)) {
                return true;
            }
        }
        return false;
    }
};

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_STRATEGIES_EARLIESTMUTUALSLOTSTRATEGY_H

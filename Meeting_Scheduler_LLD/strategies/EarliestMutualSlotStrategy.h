#ifndef MEETING_SCHEDULER_LLD_STRATEGIES_EARLIESTMUTUALSLOTSTRATEGY_H
#define MEETING_SCHEDULER_LLD_STRATEGIES_EARLIESTMUTUALSLOTSTRATEGY_H

#include <algorithm>
#include <stdexcept>
#include <vector>

#include "IFreeSlotStrategy.h"

namespace meeting_scheduler_lld {

/**
 * @class EarliestMutualSlotStrategy
 * @brief Concrete strategy jo common/mutual free slots linear search ya fixed steps ke through calculate karti hai.
 * 
 * Ye strategy step size (jaise 15 minutes) ke intervals pe check karti hai ki kya sabhi users us time bracket
 * me available hain aur unka koi clash toh nahi hai.
 */
class EarliestMutualSlotStrategy : public IFreeSlotStrategy {
public:
    // Implementation details for finding mutual free slots.
    std::vector<TimeSlot> findSlots(
        const std::string& date, int durationMinutes,
        const std::vector<std::string>& userIds,
        const std::unordered_map<std::string, std::vector<AvailabilityWindow>>& availability,
        const std::unordered_map<std::string, Meeting>& meetings) const override {
        
        if (durationMinutes <= 0) {
            throw std::invalid_argument("Duration positive honi chahiye.");
        }
        if (userIds.empty()) {
            return {};
        }

        // Sabhi users ke combined search space limits find karte hain.
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

        // Agar kisi bhi user ki availability set nahi hai toh koi slots nahi milenge.
        if (!hasAvailability) {
            return {};
        }

        std::vector<TimeSlot> slots;
        const int step = 15; // 15-minute search precision interval

        // dayStart se lekar dayEnd tak loop chala kar check karte hain.
        for (int start = dayStart; start + durationMinutes <= dayEnd; start += step) {
            const int end = start + durationMinutes;
            if (isSlotFree(date, start, end, userIds, availability, meetings)) {
                slots.emplace_back(date, start, end);
            }
        }
        return slots;
    }

private:
    // Check karta hai ki kya diya gaya slot range sabhi users ke liye free hai (R5 tracking support).
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

    // Availability verification helper.
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

    // Clash detection helper.
    bool hasMeetingConflict(const std::string& userId, const std::string& date, int start,
                            int end,
                            const std::unordered_map<std::string, Meeting>& meetings) const {
        for (const auto& entry : meetings) {
            const Meeting& meeting = entry.second;
            if (!meeting.isActive() || meeting.getDate() != date) {
                continue;
            }
            if (meeting.involvesUser(userId) && meeting.overlaps(start, end)) {
                // Agar attendee ne invitation decline kar di ho, toh unka clash nahi mana jayega.
                auto statuses = meeting.getAttendeeStatuses();
                auto it = statuses.find(userId);
                if (meeting.getOrganizerId() == userId || (it != statuses.end() && it->second != InvitationStatus::DECLINED)) {
                    return true;
                }
            }
        }
        return false;
    }
};

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_STRATEGIES_EARLIESTMUTUALSLOTSTRATEGY_H

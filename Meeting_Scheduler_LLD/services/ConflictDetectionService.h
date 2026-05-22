#ifndef MEETING_SCHEDULER_LLD_SERVICES_CONFLICTDETECTIONSERVICE_H
#define MEETING_SCHEDULER_LLD_SERVICES_CONFLICTDETECTIONSERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../models/AvailabilityWindow.h"
#include "../models/Meeting.h"

namespace meeting_scheduler_lld {

class ConflictDetectionService {
public:
    void validateSchedule(
        const std::string& date, int startMinutes, int endMinutes,
        const std::vector<std::string>& participantIds,
        const std::unordered_map<std::string, std::vector<AvailabilityWindow>>& availability,
        const std::unordered_map<std::string, Meeting>& meetings) const {
        for (const std::string& userId : participantIds) {
            if (!isWithinAvailability(userId, date, startMinutes, endMinutes, availability)) {
                throw std::runtime_error("slot outside availability for user: " + userId);
            }
            if (hasMeetingConflict(userId, date, startMinutes, endMinutes, meetings)) {
                throw std::runtime_error("meeting conflict for user: " + userId);
            }
        }
    }

private:
    bool isWithinAvailability(
        const std::string& userId, const std::string& date, int start, int end,
        const std::unordered_map<std::string, std::vector<AvailabilityWindow>>& availability)
        const {
        auto it = availability.find(userId);
        if (it == availability.end() || it->second.empty()) {
            return false;
        }
        for (const AvailabilityWindow& window : it->second) {
            if (window.getDate() == date && window.contains(start, end)) {
                return true;
            }
        }
        return false;
    }

    bool hasMeetingConflict(const std::string& userId, const std::string& date, int start, int end,
                            const std::unordered_map<std::string, Meeting>& meetings) const {
        for (const auto& entry : meetings) {
            const Meeting& meeting = entry.second;
            if (meeting.isActive() && meeting.getDate() == date && meeting.involvesUser(userId) &&
                meeting.overlaps(start, end)) {
                return true;
            }
        }
        return false;
    }
};

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_SERVICES_CONFLICTDETECTIONSERVICE_H

#ifndef MEETING_SCHEDULER_LLD_SERVICES_AVAILABILITYSERVICE_H
#define MEETING_SCHEDULER_LLD_SERVICES_AVAILABILITYSERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../models/AvailabilityWindow.h"

namespace meeting_scheduler_lld {

class AvailabilityService {
public:
    void addWindow(const std::string& userId, const std::string& date, int startMinutes,
                   int endMinutes,
                   std::unordered_map<std::string, std::vector<AvailabilityWindow>>& store) {
        AvailabilityWindow window(userId, date, startMinutes, endMinutes);
        auto& windows = store[userId];
        for (const AvailabilityWindow& existing : windows) {
            if (existing.getDate() == date && existing.overlaps(startMinutes, endMinutes)) {
                throw std::runtime_error("availability windows cannot overlap for same day");
            }
        }
        windows.push_back(window);
    }

    std::vector<AvailabilityWindow> listForUser(
        const std::string& userId, const std::string& date,
        const std::unordered_map<std::string, std::vector<AvailabilityWindow>>& store) const {
        auto it = store.find(userId);
        if (it == store.end()) {
            return {};
        }
        std::vector<AvailabilityWindow> result;
        for (const AvailabilityWindow& window : it->second) {
            if (window.getDate() == date) {
                result.push_back(window);
            }
        }
        return result;
    }
};

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_SERVICES_AVAILABILITYSERVICE_H

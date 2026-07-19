#ifndef MEETING_SCHEDULER_LLD_SERVICES_AVAILABILITYSERVICE_H
#define MEETING_SCHEDULER_LLD_SERVICES_AVAILABILITYSERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../models/AvailabilityWindow.h"

namespace meeting_scheduler_lld {

/**
 * @class AvailabilityService
 * @brief User ki availability windows ko manage karne wali helper service.
 * 
 * Naya window define karte waqt ye check karti hai ki kahin same din par koi do availability
 * windows aapas me overlap toh nahi kar rahe (e.g. 10-12 aur 11-13).
 */
class AvailabilityService {
public:
    // User ki profile me naya availability window register karne ke liye method.
    void addWindow(const std::string& userId, const std::string& date, int startMinutes,
                   int endMinutes,
                   std::unordered_map<std::string, std::vector<AvailabilityWindow>>& store) {
        AvailabilityWindow window(userId, date, startMinutes, endMinutes);
        auto& windows = store[userId];
        
        // Validation: Usi date par overlap check karna.
        for (const AvailabilityWindow& existing : windows) {
            if (existing.getDate() == date && existing.overlaps(startMinutes, endMinutes)) {
                throw std::runtime_error("Same day par availability windows overlap nahi kar sakte.");
            }
        }
        windows.push_back(window);
    }

    // Kisi user ki specific date par set ki gayi list of availability windows return karta hai.
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

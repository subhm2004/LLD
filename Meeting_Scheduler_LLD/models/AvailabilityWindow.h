#ifndef MEETING_SCHEDULER_LLD_MODELS_AVAILABILITYWINDOW_H
#define MEETING_SCHEDULER_LLD_MODELS_AVAILABILITYWINDOW_H

#include <string>

#include "../utils/TimeUtils.h"

namespace meeting_scheduler_lld {

/**
 * @class AvailabilityWindow
 * @brief Kisi ek specific date par user ke free/working hours ke time range ko represent karti hai.
 * 
 * Har user calendar par dynamic window define kar sakta hai (jaise: 9 AM to 5 PM, i.e. 540 to 1020).
 * System isi window ke logic se booking validate karta hai taaki user ko unke preferences ke bahar book na kiya jaye.
 */
class AvailabilityWindow {
public:
    AvailabilityWindow() = default;

    // Constructor jo user id aur time slot limits initialize karta hai.
    AvailabilityWindow(std::string userId, std::string date, int startMinutes, int endMinutes)
        : userId_(std::move(userId)), date_(std::move(date)) {
        validateDate(date_);
        validateTimeRange(startMinutes, endMinutes);
        startMinutes_ = startMinutes;
        endMinutes_ = endMinutes;
    }

    // User ki unique ID get karne ke liye.
    const std::string& getUserId() const { return userId_; }
    
    // Availability date get karne ke liye.
    const std::string& getDate() const { return date_; }
    
    // Start limit minutes me get karne ke liye.
    int getStartMinutes() const { return startMinutes_; }
    
    // End limit minutes me get karne ke liye.
    int getEndMinutes() const { return endMinutes_; }

    // Check karta hai ki kya ye complete slot is availability window ke andar aati hai.
    bool contains(int start, int end) const {
        return rangeContains(startMinutes_, endMinutes_, start, end);
    }

    // Check karta hai ki kya koi given slot is availability window ke sath overlap (takra) rahi hai.
    bool overlaps(int start, int end) const {
        return rangesOverlap(startMinutes_, endMinutes_, start, end);
    }

private:
    std::string userId_;         // User ki ID jiska availability window hai
    std::string date_;           // Date target
    int startMinutes_{0};        // User start time window
    int endMinutes_{0};          // User end time window
};

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_MODELS_AVAILABILITYWINDOW_H

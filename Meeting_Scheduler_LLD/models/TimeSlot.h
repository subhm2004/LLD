#ifndef MEETING_SCHEDULER_LLD_MODELS_TIMESLOT_H
#define MEETING_SCHEDULER_LLD_MODELS_TIMESLOT_H

#include <string>

#include "../utils/TimeUtils.h"

namespace meeting_scheduler_lld {

/**
 * @class TimeSlot
 * @brief Kisi calendar date par ek bookable ya free time interval ko represent karti hai.
 * 
 * Iska use slot finder algorithm ke through free slots return karne me aur calendar slots query
 * karne me kiya jata hai.
 */
class TimeSlot {
public:
    TimeSlot() = default;

    // Constructor jo time slot ko date aur duration ranges ke sath initialize karta hai.
    TimeSlot(std::string date, int startMinutes, int endMinutes)
        : date_(std::move(date)) {
        validateDate(date_);
        validateTimeRange(startMinutes, endMinutes);
        startMinutes_ = startMinutes;
        endMinutes_ = endMinutes;
    }

    // Slot ki date (YYYY-MM-DD) get karne ke liye.
    const std::string& getDate() const { return date_; }
    
    // Slot ke start time (minutes since midnight) get karne ke liye.
    int getStartMinutes() const { return startMinutes_; }
    
    // Slot ke end time (minutes since midnight) get karne ke liye.
    int getEndMinutes() const { return endMinutes_; }
    
    // Slot ki duration (minutes me) calculate karne ke liye helper method.
    int durationMinutes() const { return endMinutes_ - startMinutes_; }

private:
    std::string date_;           // Date e.g. "2026-05-25"
    int startMinutes_{0};        // Start minutes (0-1439)
    int endMinutes_{0};          // End minutes (0-1439)
};

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_MODELS_TIMESLOT_H

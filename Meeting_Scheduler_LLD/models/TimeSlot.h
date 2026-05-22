#ifndef MEETING_SCHEDULER_LLD_MODELS_TIMESLOT_H
#define MEETING_SCHEDULER_LLD_MODELS_TIMESLOT_H

#include <string>

#include "../utils/TimeUtils.h"

namespace meeting_scheduler_lld {

/** Bookable or free interval on a calendar day. */
class TimeSlot {
public:
    TimeSlot() = default;

    TimeSlot(std::string date, int startMinutes, int endMinutes)
        : date_(std::move(date)) {
        validateDate(date_);
        validateTimeRange(startMinutes, endMinutes);
        startMinutes_ = startMinutes;
        endMinutes_ = endMinutes;
    }

    const std::string& getDate() const { return date_; }
    int getStartMinutes() const { return startMinutes_; }
    int getEndMinutes() const { return endMinutes_; }
    int durationMinutes() const { return endMinutes_ - startMinutes_; }

private:
    std::string date_;
    int startMinutes_{0};
    int endMinutes_{0};
};

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_MODELS_TIMESLOT_H

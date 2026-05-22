#ifndef MEETING_SCHEDULER_LLD_MODELS_AVAILABILITYWINDOW_H
#define MEETING_SCHEDULER_LLD_MODELS_AVAILABILITYWINDOW_H

#include <string>

#include "../utils/TimeUtils.h"

namespace meeting_scheduler_lld {

class AvailabilityWindow {
public:
    AvailabilityWindow() = default;

    AvailabilityWindow(std::string userId, std::string date, int startMinutes, int endMinutes)
        : userId_(std::move(userId)), date_(std::move(date)) {
        validateDate(date_);
        validateTimeRange(startMinutes, endMinutes);
        startMinutes_ = startMinutes;
        endMinutes_ = endMinutes;
    }

    const std::string& getUserId() const { return userId_; }
    const std::string& getDate() const { return date_; }
    int getStartMinutes() const { return startMinutes_; }
    int getEndMinutes() const { return endMinutes_; }

    bool contains(int start, int end) const {
        return rangeContains(startMinutes_, endMinutes_, start, end);
    }

    bool overlaps(int start, int end) const {
        return rangesOverlap(startMinutes_, endMinutes_, start, end);
    }

private:
    std::string userId_;
    std::string date_;
    int startMinutes_{0};
    int endMinutes_{0};
};

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_MODELS_AVAILABILITYWINDOW_H

#ifndef MEETING_SCHEDULER_LLD_MODELS_MEETING_H
#define MEETING_SCHEDULER_LLD_MODELS_MEETING_H

#include <string>
#include <vector>

#include "../enums/MeetingStatus.h"
#include "../utils/TimeUtils.h"

namespace meeting_scheduler_lld {

class Meeting {
public:
    Meeting() = default;

    Meeting(std::string meetingId, std::string organizerId, std::vector<std::string> attendeeIds,
            std::string title, std::string date, int startMinutes, int endMinutes)
        : meetingId_(std::move(meetingId)),
          organizerId_(std::move(organizerId)),
          attendeeIds_(std::move(attendeeIds)),
          title_(std::move(title)),
          date_(std::move(date)),
          status_(MeetingStatus::SCHEDULED) {
        validateDate(date_);
        validateTimeRange(startMinutes, endMinutes);
        startMinutes_ = startMinutes;
        endMinutes_ = endMinutes;
    }

    const std::string& getMeetingId() const { return meetingId_; }
    const std::string& getOrganizerId() const { return organizerId_; }
    const std::vector<std::string>& getAttendeeIds() const { return attendeeIds_; }
    const std::string& getTitle() const { return title_; }
    const std::string& getDate() const { return date_; }
    int getStartMinutes() const { return startMinutes_; }
    int getEndMinutes() const { return endMinutes_; }
    MeetingStatus getStatus() const { return status_; }

    bool involvesUser(const std::string& userId) const {
        if (organizerId_ == userId) {
            return true;
        }
        for (const std::string& id : attendeeIds_) {
            if (id == userId) {
                return true;
            }
        }
        return false;
    }

    bool isActive() const { return status_ == MeetingStatus::SCHEDULED; }

    bool overlaps(int start, int end) const {
        if (!isActive()) {
            return false;
        }
        return rangesOverlap(startMinutes_, endMinutes_, start, end);
    }

    void cancel() { status_ = MeetingStatus::CANCELLED; }

private:
    std::string meetingId_;
    std::string organizerId_;
    std::vector<std::string> attendeeIds_;
    std::string title_;
    std::string date_;
    int startMinutes_{0};
    int endMinutes_{0};
    MeetingStatus status_{MeetingStatus::SCHEDULED};
};

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_MODELS_MEETING_H

#ifndef MEETING_SCHEDULER_LLD_CORE_MEETINGSCHEDULERSYSTEM_H
#define MEETING_SCHEDULER_LLD_CORE_MEETINGSCHEDULERSYSTEM_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../models/AvailabilityWindow.h"
#include "../models/Meeting.h"
#include "../models/TimeSlot.h"
#include "../models/User.h"
#include "../services/AvailabilityService.h"
#include "../services/BookingService.h"
#include "../services/ConflictDetectionService.h"
#include "../services/SlotFinderService.h"

namespace meeting_scheduler_lld {

class MeetingSchedulerSystem {
public:
    MeetingSchedulerSystem() : bookingService_(conflictService_) {}

    std::string registerUser(const std::string& name, const std::string& email,
                             const std::string& timezone = "Asia/Kolkata") {
        if (name.empty()) {
            throw std::invalid_argument("name required");
        }
        if (email.empty()) {
            throw std::invalid_argument("email required");
        }
        if (emailToUserId_.count(email) != 0) {
            throw std::runtime_error("email already registered: " + email);
        }

        const std::string userId = "MS_U" + std::to_string(++userCounter_);
        users_.emplace(userId, User(userId, name, email, timezone));
        emailToUserId_[email] = userId;
        return userId;
    }

    void setAvailability(const std::string& userId, const std::string& date, int startMinutes,
                         int endMinutes) {
        validateUser(userId);
        availabilityService_.addWindow(userId, date, startMinutes, endMinutes, availability_);
    }

    std::vector<AvailabilityWindow> getAvailability(const std::string& userId,
                                                    const std::string& date) const {
        validateUser(userId);
        return availabilityService_.listForUser(userId, date, availability_);
    }

    Meeting scheduleMeeting(const std::string& organizerId,
                            const std::vector<std::string>& attendeeIds, const std::string& title,
                            const std::string& date, int startMinutes, int endMinutes) {
        validateUser(organizerId);
        for (const std::string& id : attendeeIds) {
            validateUser(id);
        }
        return bookingService_.schedule(organizerId, attendeeIds, title, date, startMinutes,
                                        endMinutes, meetingCounter_, availability_, meetings_);
    }

    void cancelMeeting(const std::string& meetingId, const std::string& requesterId) {
        validateUser(requesterId);
        bookingService_.cancel(meetingId, requesterId, meetings_);
    }

    std::vector<Meeting> listMeetingsForUser(const std::string& userId,
                                             const std::string& date = "") const {
        validateUser(userId);
        std::vector<Meeting> result;
        for (const auto& entry : meetings_) {
            const Meeting& meeting = entry.second;
            if (!meeting.involvesUser(userId)) {
                continue;
            }
            if (!date.empty() && meeting.getDate() != date) {
                continue;
            }
            result.push_back(meeting);
        }
        return result;
    }

    std::vector<TimeSlot> findMutualFreeSlots(const std::vector<std::string>& userIds,
                                              const std::string& date, int durationMinutes) const {
        for (const std::string& id : userIds) {
            validateUser(id);
        }
        return slotFinderService_.findMutualSlots(userIds, date, durationMinutes, availability_,
                                                  meetings_);
    }

    const User& getUser(const std::string& userId) const { return getUserOrThrow(userId); }

private:
    std::unordered_map<std::string, User> users_;
    std::unordered_map<std::string, std::string> emailToUserId_;
    std::unordered_map<std::string, std::vector<AvailabilityWindow>> availability_;
    std::unordered_map<std::string, Meeting> meetings_;

    AvailabilityService availabilityService_;
    ConflictDetectionService conflictService_;
    BookingService bookingService_;
    SlotFinderService slotFinderService_;

    int userCounter_{0};
    int meetingCounter_{0};

    void validateUser(const std::string& userId) const {
        if (users_.find(userId) == users_.end()) {
            throw std::runtime_error("user not found: " + userId);
        }
    }

    const User& getUserOrThrow(const std::string& userId) const {
        auto it = users_.find(userId);
        if (it == users_.end()) {
            throw std::runtime_error("user not found: " + userId);
        }
        return it->second;
    }
};

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_CORE_MEETINGSCHEDULERSYSTEM_H

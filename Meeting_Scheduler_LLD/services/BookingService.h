#ifndef MEETING_SCHEDULER_LLD_SERVICES_BOOKINGSERVICE_H
#define MEETING_SCHEDULER_LLD_SERVICES_BOOKINGSERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../factories/MeetingFactory.h"
#include "../models/AvailabilityWindow.h"
#include "../models/Meeting.h"
#include "ConflictDetectionService.h"

namespace meeting_scheduler_lld {

class BookingService {
public:
    explicit BookingService(ConflictDetectionService& conflictService)
        : conflictService_(conflictService) {}

    Meeting schedule(const std::string& organizerId, const std::vector<std::string>& attendeeIds,
                     const std::string& title, const std::string& date, int startMinutes,
                     int endMinutes, int& meetingCounter,
                     const std::unordered_map<std::string, std::vector<AvailabilityWindow>>&
                         availability,
                     std::unordered_map<std::string, Meeting>& meetings) {
        if (attendeeIds.empty()) {
            throw std::invalid_argument("at least one attendee required");
        }

        std::vector<std::string> participants;
        participants.push_back(organizerId);
        for (const std::string& id : attendeeIds) {
            if (id == organizerId) {
                continue;
            }
            participants.push_back(id);
        }

        conflictService_.validateSchedule(date, startMinutes, endMinutes, participants,
                                        availability, meetings);

        const std::string meetingId = "MTG_" + std::to_string(++meetingCounter);
        Meeting meeting =
            MeetingFactory::create(meetingId, organizerId, attendeeIds, title, date, startMinutes,
                                   endMinutes);
        meetings.emplace(meetingId, meeting);
        return meeting;
    }

    void cancel(const std::string& meetingId, const std::string& requesterId,
                std::unordered_map<std::string, Meeting>& meetings) {
        auto it = meetings.find(meetingId);
        if (it == meetings.end()) {
            throw std::runtime_error("meeting not found");
        }
        Meeting& meeting = it->second;
        if (!meeting.involvesUser(requesterId)) {
            throw std::runtime_error("only participants can cancel");
        }
        if (!meeting.isActive()) {
            throw std::runtime_error("meeting already cancelled");
        }
        meeting.cancel();
    }

private:
    ConflictDetectionService& conflictService_;
};

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_SERVICES_BOOKINGSERVICE_H

#ifndef MEETING_SCHEDULER_LLD_SERVICES_BOOKINGSERVICE_H
#define MEETING_SCHEDULER_LLD_SERVICES_BOOKINGSERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../factories/MeetingFactory.h"
#include "../models/AvailabilityWindow.h"
#include "../models/Meeting.h"
#include "../models/MeetingRoom.h"
#include "../models/User.h"
#include "ConflictDetectionService.h"
#include "NotificationService.h"

namespace meeting_scheduler_lld {

/**
 * @class BookingService
 * @brief Meeting booking schedule karne aur cancel karne wali service.
 * 
 * Ye service direct Factory se meeting object create karti hai,
 * user calendars ko update karti hai (R6), meeting rooms lock karti hai (R1, R2, R3),
 * aur actions par Notification Service ke through notifications broadcast karti hai (R4).
 */
class BookingService {
public:
    // Constructor jo Conflict Detection helper service save karta hai.
    explicit BookingService(ConflictDetectionService& conflictService)
        : conflictService_(conflictService) {}

    // Meeting schedule karne ke liye main function.
    Meeting schedule(const std::string& organizerId, const std::vector<std::string>& attendeeIds,
                     const std::string& title, const std::string& date, int startMinutes,
                     int endMinutes, const std::string& roomId, bool strictMode, int& meetingCounter,
                     std::unordered_map<std::string, User>& users,
                     const std::unordered_map<std::string, MeetingRoom>& rooms,
                     const std::unordered_map<std::string, std::vector<AvailabilityWindow>>& availability,
                     std::unordered_map<std::string, Meeting>& meetings,
                     NotificationService& notificationService) {
        
        if (attendeeIds.empty()) {
            throw std::invalid_argument("Kam se kam ek attendee hona jaruri hai");
        }

        // Participant count = organizer + total attendees
        int totalParticipants = 1 + attendeeIds.size();

        // 1. Room assignment logic.
        // Agar clients ne roomId specify nahi kiya toh automatic free room allocate karein.
        std::string assignedRoomId = roomId;
        if (assignedRoomId.empty()) {
            bool roomFound = false;
            for (const auto& entry : rooms) {
                const MeetingRoom& r = entry.second;
                if (r.canAccommodate(totalParticipants)) {
                    // Check ki kya room is specific timing pe busy hai.
                    bool hasConflict = false;
                    for (const auto& mEntry : meetings) {
                        const Meeting& m = mEntry.second;
                        if (m.isActive() && m.getRoomId() == r.getRoomId() && m.getDate() == date && m.overlaps(startMinutes, endMinutes)) {
                            hasConflict = true;
                            break;
                        }
                    }
                    if (!hasConflict) {
                        assignedRoomId = r.getRoomId();
                        roomFound = true;
                        break;
                    }
                }
            }
            if (!roomFound) {
                throw std::runtime_error("Time slot " + date + " " + std::to_string(startMinutes) + "-" + std::to_string(endMinutes) + " par koi bhi room free ya sufficient capacity ka nahi mila.");
            }
        } else {
            // Agar room specify kiya hai toh validations run karein.
            conflictService_.validateRoom(assignedRoomId, totalParticipants, date, startMinutes, endMinutes, rooms, meetings);
        }

        // 2. Schedule conflicts and availability validation.
        conflictService_.validateSchedule(date, startMinutes, endMinutes, organizerId, attendeeIds,
                                          availability, meetings, strictMode);

        // 3. Create unique meeting ID and create meeting object via Factory pattern.
        const std::string meetingId = "MTG_" + std::to_string(++meetingCounter);
        Meeting meeting = MeetingFactory::create(meetingId, organizerId, attendeeIds, title, date,
                                                 startMinutes, endMinutes, assignedRoomId);

        // If flexible scheduling (strictMode = false), set organizer as ACCEPTED automatically.
        // Organizer by default accepted status par rahega.
        
        // 4. Update the Calendar of each participant.
        // Organizer Calendar update
        auto orgIt = users.find(organizerId);
        if (orgIt != users.end()) {
            orgIt->second.getCalendar().addMeeting(meetingId);
        }

        // Attendees Calendar updates
        for (const std::string& attId : attendeeIds) {
            auto attIt = users.find(attId);
            if (attIt != users.end()) {
                attIt->second.getCalendar().addMeeting(meetingId);
            }
        }

        // Save meeting to memory storage.
        meetings.emplace(meetingId, meeting);

        // 5. Broadcast schedule notifications (Observer Pattern).
        notificationService.notifyScheduled(meeting);

        return meeting;
    }

    // Meeting cancel karne ke liye function.
    void cancel(const std::string& meetingId, const std::string& requesterId,
                std::unordered_map<std::string, User>& users,
                std::unordered_map<std::string, Meeting>& meetings,
                NotificationService& notificationService) {
        
        auto it = meetings.find(meetingId);
        if (it == meetings.end()) {
            throw std::runtime_error("Meeting nahi mili: " + meetingId);
        }

        Meeting& meeting = it->second;
        if (!meeting.involvesUser(requesterId)) {
            throw std::runtime_error("Sirf meeting participants hi cancel kar sakte hain");
        }
        if (!meeting.isActive()) {
            throw std::runtime_error("Meeting already cancel ho chuki hai");
        }

        // Status update to cancel
        meeting.cancel();

        // Har user ke calendar se is meeting ko update/remove karein.
        // Organizer calendar update
        auto orgIt = users.find(meeting.getOrganizerId());
        if (orgIt != users.end()) {
            orgIt->second.getCalendar().removeMeeting(meetingId);
        }

        // Attendees calendar update
        for (const std::string& attId : meeting.getAttendeeIds()) {
            auto attIt = users.find(attId);
            if (attIt != users.end()) {
                attIt->second.getCalendar().removeMeeting(meetingId);
            }
        }

        // Notify all participants about cancellation.
        notificationService.notifyCancelled(meeting, requesterId);
    }

private:
    ConflictDetectionService& conflictService_;
};

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_SERVICES_BOOKINGSERVICE_H

#ifndef MEETING_SCHEDULER_LLD_CORE_MEETINGSCHEDULERSYSTEM_H
#define MEETING_SCHEDULER_LLD_CORE_MEETINGSCHEDULERSYSTEM_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "../models/AvailabilityWindow.h"
#include "../models/Meeting.h"
#include "../models/MeetingRoom.h"
#include "../models/TimeSlot.h"
#include "../models/User.h"
#include "../services/AvailabilityService.h"
#include "../services/BookingService.h"
#include "../services/ConflictDetectionService.h"
#include "../services/SlotFinderService.h"
#include "../services/NotificationService.h"

namespace meeting_scheduler_lld {

/**
 * @class MeetingSchedulerSystem
 * @brief System ka main Facade (Facade Design Pattern).
 * 
 * Client code is class ke methods ko call karke users register kar sakta hai, availability set
 * kar sakta hai, rooms manage kar sakta hai, meetings book aur cancel kar sakta hai, aur 
 * notification observers add kar sakta hai.
 */
class MeetingSchedulerSystem {
public:
    // Facade Constructor jo dependency inject/link karta hai services ke bich.
    MeetingSchedulerSystem() : bookingService_(conflictService_) {}

    // User register karne ke liye method.
    std::string registerUser(const std::string& name, const std::string& email,
                             const std::string& timezone = "Asia/Kolkata") {
        if (name.empty()) {
            throw std::invalid_argument("User name blank nahi ho sakta");
        }
        if (email.empty()) {
            throw std::invalid_argument("Email mandatory hai");
        }
        if (emailToUserId_.count(email) != 0) {
            throw std::runtime_error("Email already registered hai: " + email);
        }

        const std::string userId = "MS_U" + std::to_string(++userCounter_);
        users_.emplace(userId, User(userId, name, email, timezone));
        emailToUserId_[email] = userId;
        return userId;
    }

    // Availability set karne aur user calendar sync karne ke liye (R6, FR2).
    void setAvailability(const std::string& userId, const std::string& date, int startMinutes,
                         int endMinutes) {
        validateUser(userId);
        availabilityService_.addWindow(userId, date, startMinutes, endMinutes, availability_);
        
        // personal calendar update
        users_[userId].getCalendar().addAvailability(AvailabilityWindow(userId, date, startMinutes, endMinutes));
    }

    // Target user ki static calendar availability slots get karne ke liye.
    std::vector<AvailabilityWindow> getAvailability(const std::string& userId,
                                                    const std::string& date) const {
        validateUser(userId);
        return availabilityService_.listForUser(userId, date, availability_);
    }

    // Naya Meeting Room system me register karne ke liye (R1, R2 support).
    void addMeetingRoom(const std::string& roomId, const std::string& name, int capacity) {
        if (roomId.empty() || name.empty()) {
            throw std::invalid_argument("Room ID aur Name mandatory hain");
        }
        if (rooms_.count(roomId) > 0) {
            throw std::runtime_error("Room ID already registered hai: " + roomId);
        }
        rooms_.emplace(roomId, MeetingRoom(roomId, name, capacity));
    }

    // Register kiye gaye meeting rooms map return karne ke liye helper getter.
    const std::unordered_map<std::string, MeetingRoom>& getMeetingRooms() const {
        return rooms_;
    }

    // Notification observer register karne ke liye callback helper (R4, Observer Pattern).
    void registerNotificationObserver(std::shared_ptr<INotificationObserver> observer) {
        notificationService_.registerObserver(observer);
    }

    // Meeting book schedule karne ke liye facade endpoint.
    // Backward compatibility ke liye defaults parameter settings applied.
    Meeting scheduleMeeting(const std::string& organizerId,
                            const std::vector<std::string>& attendeeIds, const std::string& title,
                            const std::string& date, int startMinutes, int endMinutes,
                            const std::string& roomId = "", bool strictMode = true) {
        validateUser(organizerId);
        for (const std::string& id : attendeeIds) {
            validateUser(id);
        }
        return bookingService_.schedule(organizerId, attendeeIds, title, date, startMinutes,
                                        endMinutes, roomId, strictMode, meetingCounter_,
                                        users_, rooms_, availability_, meetings_, notificationService_);
    }

    // Meeting cancel karne ke liye endpoint.
    void cancelMeeting(const std::string& meetingId, const std::string& requesterId) {
        validateUser(requesterId);
        bookingService_.cancel(meetingId, requesterId, users_, meetings_, notificationService_);
    }

    // Active/Cancelled meetings history user profile ke filters par return karne ke liye.
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

    // Multiple attendees ke liye common mutual free intervals calculate karne ke liye strategy search execution.
    std::vector<TimeSlot> findMutualFreeSlots(const std::vector<std::string>& userIds,
                                               const std::string& date, int durationMinutes) const {
        for (const std::string& id : userIds) {
            validateUser(id);
        }
        return slotFinderService_.findMutualSlots(userIds, date, durationMinutes, availability_,
                                                  meetings_);
    }

    // Individual attendee ke incoming meeting invitation ko respond (Accept/Decline) karne ke liye method (R5).
    void respondToInvitation(const std::string& meetingId, const std::string& userId, InvitationStatus status) {
        validateUser(userId);
        auto it = meetings_.find(meetingId);
        if (it == meetings_.end()) {
            throw std::runtime_error("Meeting nahi mili: " + meetingId);
        }
        Meeting& meeting = it->second;
        if (!meeting.involvesUser(userId)) {
            throw std::runtime_error("User is meeting me invited nahi hai: " + userId);
        }
        if (!meeting.isActive()) {
            throw std::runtime_error("Meeting active nahi hai.");
        }

        if (status == InvitationStatus::ACCEPTED) {
            // Confirm ki kya user ke paas already koi active overlap conflict to nahi hai accept karte waqt.
            if (conflictService_.hasMeetingConflict(userId, meeting.getDate(), meeting.getStartMinutes(), meeting.getEndMinutes(), meetings_, meetingId)) {
                throw std::runtime_error("Accept failed: User has overlapping confirmed meeting conflict at this time slot.");
            }
            meeting.setAttendeeStatus(userId, InvitationStatus::ACCEPTED);
        } else if (status == InvitationStatus::DECLINED) {
            meeting.setAttendeeStatus(userId, InvitationStatus::DECLINED);
        }

        // Broadcaster notify events for status change
        notificationService_.notifyInvitationUpdated(meeting, userId, status);
    }

    // User lookup helper check.
    const User& getUser(const std::string& userId) const { 
        return getUserOrThrow(userId); 
    }

private:
    std::unordered_map<std::string, User> users_;                                           // DB mock store of Users
    std::unordered_map<std::string, std::string> emailToUserId_;                            // email lookup index
    std::unordered_map<std::string, MeetingRoom> rooms_;                                    // DB mock store of MeetingRooms (R1)
    std::unordered_map<std::string, std::vector<AvailabilityWindow>> availability_;         // availability index
    std::unordered_map<std::string, Meeting> meetings_;                                     // DB mock store of Meetings

    AvailabilityService availabilityService_;
    ConflictDetectionService conflictService_;
    BookingService bookingService_;
    SlotFinderService slotFinderService_;
    NotificationService notificationService_;                                                // Observer broadcast subject (R4)

    int userCounter_{0};
    int meetingCounter_{0};

    // User verification function.
    void validateUser(const std::string& userId) const {
        if (users_.find(userId) == users_.end()) {
            throw std::runtime_error("User profile nahi mili: " + userId);
        }
    }

    // Throw safe retrieval of User profile reference.
    const User& getUserOrThrow(const std::string& userId) const {
        auto it = users_.find(userId);
        if (it == users_.end()) {
            throw std::runtime_error("User profile nahi mili: " + userId);
        }
        return it->second;
    }
};

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_CORE_MEETINGSCHEDULERSYSTEM_H

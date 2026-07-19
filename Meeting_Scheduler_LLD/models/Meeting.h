#ifndef MEETING_SCHEDULER_LLD_MODELS_MEETING_H
#define MEETING_SCHEDULER_LLD_MODELS_MEETING_H

#include <string>
#include <vector>
#include <unordered_map>

#include "../enums/MeetingStatus.h"
#include "../enums/InvitationStatus.h"
#include "../utils/TimeUtils.h"

namespace meeting_scheduler_lld {

/**
 * @class Meeting
 * @brief Ek schedule ki gayi meeting ko represent karti hai.
 * 
 * Isme meeting ki time details (date, start & end minutes), organizer, invitees, assign kiya gaya
 * meeting room, aur har attendee ke invitation status (PENDING, ACCEPTED, DECLINED) ko track kiya jata hai.
 */
class Meeting {
public:
    Meeting() = default;

    // Constructor jo meeting ki details set karta hai aur default values initialize karta hai.
    Meeting(std::string meetingId, std::string organizerId, std::vector<std::string> attendeeIds,
            std::string title, std::string date, int startMinutes, int endMinutes, std::string roomId)
        : meetingId_(std::move(meetingId)),
          organizerId_(std::move(organizerId)),
          attendeeIds_(std::move(attendeeIds)),
          title_(std::move(title)),
          date_(std::move(date)),
          roomId_(std::move(roomId)),
          status_(MeetingStatus::SCHEDULED) {
        validateDate(date_);
        validateTimeRange(startMinutes, endMinutes);
        startMinutes_ = startMinutes;
        endMinutes_ = endMinutes;

        // Har ek invited attendee ka initial status PENDING set karte hain.
        for (const std::string& attendeeId : attendeeIds_) {
            attendeeStatuses_[attendeeId] = InvitationStatus::PENDING;
        }
    }

    // Getters for meeting properties
    const std::string& getMeetingId() const { return meetingId_; }
    const std::string& getOrganizerId() const { return organizerId_; }
    const std::vector<std::string>& getAttendeeIds() const { return attendeeIds_; }
    const std::string& getTitle() const { return title_; }
    const std::string& getDate() const { return date_; }
    int getStartMinutes() const { return startMinutes_; }
    int getEndMinutes() const { return endMinutes_; }
    MeetingStatus getStatus() const { return status_; }
    const std::string& getRoomId() const { return roomId_; }
    const std::unordered_map<std::string, InvitationStatus>& getAttendeeStatuses() const { return attendeeStatuses_; }

    // Check karta hai ki kya koi particular user is meeting ka part hai (organizer ya attendee).
    bool involvesUser(const std::string& userId) const {
        if (organizerId_ == userId) {
            return true;
        }
        return attendeeStatuses_.count(userId) > 0;
    }

    // Meeting active hai ya nahi check karne ke liye.
    bool isActive() const { return status_ == MeetingStatus::SCHEDULED; }

    // Check karta hai ki kya ye meeting kisi aur time interval ke sath overlap karti hai.
    bool overlaps(int start, int end) const {
        if (!isActive()) {
            return false;
        }
        return rangesOverlap(startMinutes_, endMinutes_, start, end);
    }

    // Meeting cancel karne ke liye status ko CANCELLED me update karta hai.
    void cancel() { 
        status_ = MeetingStatus::CANCELLED; 
    }

    // Kisi attendee ka invitation status (ACCEPTED/DECLINED) change karne ke liye.
    void setAttendeeStatus(const std::string& userId, InvitationStatus status) {
        auto it = attendeeStatuses_.find(userId);
        if (it != attendeeStatuses_.end()) {
            it->second = status;
        }
    }

private:
    std::string meetingId_;       // Meeting ki unique ID (e.g., "MTG_1")
    std::string organizerId_;     // Organizer ki user ID (e.g., "MS_U1")
    std::vector<std::string> attendeeIds_; // Invited attendees ki user IDs list
    std::string title_;           // Meeting ka title
    std::string date_;            // Date YYYY-MM-DD
    int startMinutes_{0};         // Start time in minutes from midnight (0-1439)
    int endMinutes_{0};           // End time in minutes from midnight (0-1439)
    std::string roomId_;          // Booked meeting room ID (e.g., "ROOM_1")
    MeetingStatus status_{MeetingStatus::SCHEDULED}; // Meeting status (SCHEDULED/CANCELLED)

    // Har ek attendee ke response state ko maintain karne ke liye map.
    std::unordered_map<std::string, InvitationStatus> attendeeStatuses_;
};

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_MODELS_MEETING_H

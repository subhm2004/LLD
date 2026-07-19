#ifndef MEETING_SCHEDULER_LLD_SERVICES_CONFLICTDETECTIONSERVICE_H
#define MEETING_SCHEDULER_LLD_SERVICES_CONFLICTDETECTIONSERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../models/AvailabilityWindow.h"
#include "../models/Meeting.h"
#include "../models/MeetingRoom.h"

namespace meeting_scheduler_lld {

/**
 * @class ConflictDetectionService
 * @brief Meeting booking ke conflicts detect aur validate karne wali service.
 * 
 * Ye service do mukhya kaam karti hai:
 * 1. Room availability aur seating capacity check karti hai.
 * 2. Organizer aur attendees ki schedule overlap aur availability check karti hai.
 * 
 * Hinglish comments:
 * - Isme validateRoom aur validateSchedule functions added hain jo conditions fail hone par runtime_error throw karte hain.
 */
class ConflictDetectionService {
public:
    // Meeting Room ki conditions validate karne ke liye.
    // R1, R2, R3 Requirements verification.
    void validateRoom(
        const std::string& roomId, int totalParticipants, const std::string& date,
        int startMinutes, int endMinutes,
        const std::unordered_map<std::string, MeetingRoom>& rooms,
        const std::unordered_map<std::string, Meeting>& meetings) const {
        
        // Check 1: Room registered hai ya nahi.
        auto it = rooms.find(roomId);
        if (it == rooms.end()) {
            throw std::runtime_error("Meeting room exist nahi karta: " + roomId);
        }
        
        // Check 2: Seating capacity checking.
        const MeetingRoom& room = it->second;
        if (!room.canAccommodate(totalParticipants)) {
            throw std::runtime_error("Room capacity insufficient hai. Required: " + 
                                     std::to_string(totalParticipants) + ", Available capacity: " + 
                                     std::to_string(room.getCapacity()));
        }

        // Check 3: Overlapping active booking of this room.
        for (const auto& entry : meetings) {
            const Meeting& m = entry.second;
            if (m.isActive() && m.getRoomId() == roomId && m.getDate() == date && m.overlaps(startMinutes, endMinutes)) {
                throw std::runtime_error("Meeting room " + roomId + " is time slot par already booked hai by meeting: " + m.getMeetingId());
            }
        }
    }

    // Schedule aur participants ki limits ko validate karne ke liye.
    void validateSchedule(
        const std::string& date, int startMinutes, int endMinutes,
        const std::string& organizerId,
        const std::vector<std::string>& attendeeIds,
        const std::unordered_map<std::string, std::vector<AvailabilityWindow>>& availability,
        const std::unordered_map<std::string, Meeting>& meetings,
        bool strictMode) const {

        // Check 1: Organizer ka busy check. Organizer humesha available time frame me hona chahiye aur free hona chahiye.
        if (!isWithinAvailability(organizerId, date, startMinutes, endMinutes, availability)) {
            throw std::runtime_error("Slot outside availability window hai for organizer: " + organizerId);
        }
        if (hasMeetingConflict(organizerId, date, startMinutes, endMinutes, meetings)) {
            throw std::runtime_error("Organizer has overlapping meeting conflict: " + organizerId);
        }

        // Check 2: Attendees verification.
        for (const std::string& attendeeId : attendeeIds) {
            if (attendeeId == organizerId) {
                continue;
            }
            
            bool available = isWithinAvailability(attendeeId, date, startMinutes, endMinutes, availability);
            bool conflict = hasMeetingConflict(attendeeId, date, startMinutes, endMinutes, meetings);

            // Agar strictMode true hai toh failure par throw karenge (e.g. standard schedule flow).
            // Agar strictMode false hai toh throw nahi karenge, user ko pending invitation mil jayegi aur bad me manage karenge.
            if (strictMode) {
                if (!available) {
                    throw std::runtime_error("Slot outside availability window hai for attendee: " + attendeeId);
                }
                if (conflict) {
                    throw std::runtime_error("Attendee has overlapping meeting conflict: " + attendeeId);
                }
            }
        }
    }

    // Check karta hai ki kya user kisi overlapping active meeting me hai (jo DECLINED nahi hai).
    bool hasMeetingConflict(const std::string& userId, const std::string& date, int start, int end,
                            const std::unordered_map<std::string, Meeting>& meetings,
                            const std::string& meetingIdToSkip = "") const {
        for (const auto& entry : meetings) {
            const Meeting& meeting = entry.second;
            if (!meetingIdToSkip.empty() && meeting.getMeetingId() == meetingIdToSkip) {
                continue;
            }
            if (meeting.isActive() && meeting.getDate() == date && meeting.involvesUser(userId) &&
                meeting.overlaps(start, end)) {
                
                // Agar attendee ne invite DECLINE kar diya hai toh clash nahi mana jayega.
                auto statuses = meeting.getAttendeeStatuses();
                auto it = statuses.find(userId);
                if (meeting.getOrganizerId() == userId || (it != statuses.end() && it->second != InvitationStatus::DECLINED)) {
                    return true;
                }
            }
        }
        return false;
    }

    // Check karta hai ki kya slot complete user availability window ke under fall karta hai.
    bool isWithinAvailability(
        const std::string& userId, const std::string& date, int start, int end,
        const std::unordered_map<std::string, std::vector<AvailabilityWindow>>& availability)
        const {
        auto it = availability.find(userId);
        if (it == availability.end() || it->second.empty()) {
            return false;
        }
        for (const AvailabilityWindow& window : it->second) {
            if (window.getDate() == date && window.contains(start, end)) {
                return true;
            }
        }
        return false;
    }
};

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_SERVICES_CONFLICTDETECTIONSERVICE_H

#ifndef MEETING_SCHEDULER_LLD_MODELS_MEETINGROOM_H
#define MEETING_SCHEDULER_LLD_MODELS_MEETINGROOM_H

#include <string>

namespace meeting_scheduler_lld {

/**
 * @class MeetingRoom
 * @brief Ek physical meeting room ko represent karti hai jahan meeting hoti hai.
 * 
 * Har room ki apni specific capacity hoti hai (ki usme kitne log baith sakte hain).
 * Agar meeting ke total participants (organizer + attendees) room ki capacity se jyada honge,
 * toh booking allow nahi ki jayegi (Rule checking).
 */
class MeetingRoom {
public:
    MeetingRoom() = default;

    MeetingRoom(std::string roomId, std::string name, int capacity)
        : roomId_(std::move(roomId)),
          name_(std::move(name)),
          capacity_(capacity) {}

    // Getters for room properties
    const std::string& getRoomId() const { return roomId_; }
    const std::string& getName() const { return name_; }
    int getCapacity() const { return capacity_; }

    // Check karta hai ki kya ye room itne logon ko hold kar sakta hai ya nahi.
    bool canAccommodate(int numPeople) const {
        return capacity_ >= numPeople;
    }

private:
    std::string roomId_;     // Room ki unique identifier (e.g., "ROOM_1")
    std::string name_;       // Room ka friendly name (e.g., "Turing Room")
    int capacity_{0};        // Room me maximum allowed log (capacity)
};

} // namespace meeting_scheduler_lld

#endif // MEETING_SCHEDULER_LLD_MODELS_MEETINGROOM_H

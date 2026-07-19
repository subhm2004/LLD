#ifndef MEETING_SCHEDULER_LLD_FACTORIES_MEETINGFACTORY_H
#define MEETING_SCHEDULER_LLD_FACTORIES_MEETINGFACTORY_H

#include <string>
#include <vector>

#include "../models/Meeting.h"

namespace meeting_scheduler_lld {

/**
 * @class MeetingFactory
 * @brief Factory Design Pattern ka use karke Meeting objects create karne ki class.
 * 
 * Client code ko direct `Meeting` class ke constructor ko call karne ki jarurat nahi hai.
 * Ye factory class `Meeting` object instantiation ki complexity ko abstract karti hai.
 */
class MeetingFactory {
public:
    // Nayi meeting create karne ke liye static method jo simple drop-in instance return karta hai.
    static Meeting create(const std::string& meetingId, const std::string& organizerId,
                          const std::vector<std::string>& attendeeIds, const std::string& title,
                          const std::string& date, int startMinutes, int endMinutes, const std::string& roomId) {
        return Meeting(meetingId, organizerId, attendeeIds, title, date, startMinutes, endMinutes, roomId);
    }
};

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_FACTORIES_MEETINGFACTORY_H

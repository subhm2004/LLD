#ifndef MEETING_SCHEDULER_LLD_FACTORIES_MEETINGFACTORY_H
#define MEETING_SCHEDULER_LLD_FACTORIES_MEETINGFACTORY_H

#include <string>
#include <vector>

#include "../models/Meeting.h"

namespace meeting_scheduler_lld {

class MeetingFactory {
public:
    static Meeting create(const std::string& meetingId, const std::string& organizerId,
                          const std::vector<std::string>& attendeeIds, const std::string& title,
                          const std::string& date, int startMinutes, int endMinutes) {
        return Meeting(meetingId, organizerId, attendeeIds, title, date, startMinutes, endMinutes);
    }
};

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_FACTORIES_MEETINGFACTORY_H

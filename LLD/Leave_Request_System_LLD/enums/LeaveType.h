#ifndef LEAVE_REQUEST_SYSTEM_LLD_ENUMS_LEAVE_TYPE_H
#define LEAVE_REQUEST_SYSTEM_LLD_ENUMS_LEAVE_TYPE_H

#include <string>

namespace leave_request_lld {

enum class LeaveType { CASUAL, SICK, EARNED, UNPAID };

inline std::string leaveTypeToString(LeaveType type) {
    switch (type) {
        case LeaveType::CASUAL:
            return "CASUAL";
        case LeaveType::SICK:
            return "SICK";
        case LeaveType::EARNED:
            return "EARNED";
        case LeaveType::UNPAID:
            return "UNPAID";
        default:
            return "UNKNOWN";
    }
}

}  // namespace leave_request_lld

#endif  // LEAVE_REQUEST_SYSTEM_LLD_ENUMS_LEAVE_TYPE_H

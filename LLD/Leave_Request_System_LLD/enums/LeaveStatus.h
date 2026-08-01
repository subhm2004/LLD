#ifndef LEAVE_REQUEST_SYSTEM_LLD_ENUMS_LEAVE_STATUS_H
#define LEAVE_REQUEST_SYSTEM_LLD_ENUMS_LEAVE_STATUS_H

#include <string>

namespace leave_request_lld {

enum class LeaveStatus { PENDING, APPROVED, REJECTED, CANCELLED };

inline std::string leaveStatusToString(LeaveStatus status) {
    switch (status) {
        case LeaveStatus::PENDING:
            return "PENDING";
        case LeaveStatus::APPROVED:
            return "APPROVED";
        case LeaveStatus::REJECTED:
            return "REJECTED";
        case LeaveStatus::CANCELLED:
            return "CANCELLED";
        default:
            return "UNKNOWN";
    }
}

}  // namespace leave_request_lld

#endif  // LEAVE_REQUEST_SYSTEM_LLD_ENUMS_LEAVE_STATUS_H

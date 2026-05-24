#ifndef LEAVE_REQUEST_SYSTEM_LLD_ENUMS_APPROVER_ROLE_H
#define LEAVE_REQUEST_SYSTEM_LLD_ENUMS_APPROVER_ROLE_H

#include <string>

namespace leave_request_lld {

enum class ApproverRole { TEAM_LEAD, MANAGER, HR, DIRECTOR, NONE };

inline std::string approverRoleToString(ApproverRole role) {
    switch (role) {
        case ApproverRole::TEAM_LEAD:
            return "TEAM_LEAD";
        case ApproverRole::MANAGER:
            return "MANAGER";
        case ApproverRole::HR:
            return "HR";
        case ApproverRole::DIRECTOR:
            return "DIRECTOR";
        case ApproverRole::NONE:
            return "NONE";
        default:
            return "UNKNOWN";
    }
}

}  // namespace leave_request_lld

#endif  // LEAVE_REQUEST_SYSTEM_LLD_ENUMS_APPROVER_ROLE_H

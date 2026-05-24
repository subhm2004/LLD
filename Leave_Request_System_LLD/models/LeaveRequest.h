#ifndef LEAVE_REQUEST_SYSTEM_LLD_MODELS_LEAVE_REQUEST_H
#define LEAVE_REQUEST_SYSTEM_LLD_MODELS_LEAVE_REQUEST_H

#include <string>

#include "../enums/ApproverRole.h"
#include "../enums/LeaveStatus.h"
#include "../enums/LeaveType.h"

namespace leave_request_lld {

struct LeaveRequest {
    std::string requestId;
    std::string employeeId;
    LeaveType leaveType{LeaveType::CASUAL};
    std::string startDate;
    std::string endDate;
    int workingDays{0};
    LeaveStatus status{LeaveStatus::PENDING};
    ApproverRole approvedBy{ApproverRole::NONE};
    std::string remarks;
};

}  // namespace leave_request_lld

#endif  // LEAVE_REQUEST_SYSTEM_LLD_MODELS_LEAVE_REQUEST_H

#ifndef LEAVE_REQUEST_SYSTEM_LLD_HANDLERS_HR_HANDLER_H
#define LEAVE_REQUEST_SYSTEM_LLD_HANDLERS_HR_HANDLER_H

#include <iostream>

#include "LeaveApprovalHandler.h"

namespace leave_request_lld {

class HRHandler : public LeaveApprovalHandler {
protected:
    bool canApprove(const LeaveRequest& request) const override {
        return request.workingDays <= maxDays();
    }

    void approve(LeaveRequest& request) const override {
        request.status = LeaveStatus::APPROVED;
        request.approvedBy = role();
        request.remarks = "Approved by HR (<= " + std::to_string(maxDays()) + " days)";
        std::cout << "  [CoR] HR approved " << request.requestId << " ("
                  << request.workingDays << " days)\n";
    }

    ApproverRole role() const override { return ApproverRole::HR; }
    int maxDays() const override { return 7; }
};

}  // namespace leave_request_lld

#endif  // LEAVE_REQUEST_SYSTEM_LLD_HANDLERS_HR_HANDLER_H

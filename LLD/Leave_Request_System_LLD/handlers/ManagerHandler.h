#ifndef LEAVE_REQUEST_SYSTEM_LLD_HANDLERS_MANAGER_HANDLER_H
#define LEAVE_REQUEST_SYSTEM_LLD_HANDLERS_MANAGER_HANDLER_H

#include <iostream>

#include "LeaveApprovalHandler.h"

namespace leave_request_lld {

class ManagerHandler : public LeaveApprovalHandler {
protected:
    bool canApprove(const LeaveRequest& request) const override {
        return request.workingDays <= maxDays();
    }

    void approve(LeaveRequest& request) const override {
        request.status = LeaveStatus::APPROVED;
        request.approvedBy = role();
        request.remarks = "Approved by Manager (<= " + std::to_string(maxDays()) + " days)";
        std::cout << "  [CoR] Manager approved " << request.requestId << " ("
                  << request.workingDays << " days)\n";
    }

    ApproverRole role() const override { return ApproverRole::MANAGER; }
    int maxDays() const override { return 3; }
};

}  // namespace leave_request_lld

#endif  // LEAVE_REQUEST_SYSTEM_LLD_HANDLERS_MANAGER_HANDLER_H

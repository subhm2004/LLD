#ifndef LEAVE_REQUEST_SYSTEM_LLD_HANDLERS_DIRECTOR_HANDLER_H
#define LEAVE_REQUEST_SYSTEM_LLD_HANDLERS_DIRECTOR_HANDLER_H

#include <iostream>
#include "LeaveApprovalHandler.h"

namespace leave_request_lld {

class DirectorHandler : public LeaveApprovalHandler {
protected:
    bool canApprove(const LeaveRequest& request) const override {
        return request.workingDays <= maxDays();
    }

    void approve(LeaveRequest& request) const override {
        request.status = LeaveStatus::APPROVED;
        request.approvedBy = role();
        request.remarks = "Approved by Director (executive sign-off)";
        std::cout << "  [CoR] Director approved " << request.requestId << " ("
                  << request.workingDays << " days)\n";
    }

    ApproverRole role() const override { return ApproverRole::DIRECTOR; }
    int maxDays() const override { return 30; }  // executive cap; beyond → chain rejects
};

}  // namespace leave_request_lld

#endif  // LEAVE_REQUEST_SYSTEM_LLD_HANDLERS_DIRECTOR_HANDLER_H

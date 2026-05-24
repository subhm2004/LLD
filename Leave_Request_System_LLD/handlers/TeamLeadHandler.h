#ifndef LEAVE_REQUEST_SYSTEM_LLD_HANDLERS_TEAM_LEAD_HANDLER_H
#define LEAVE_REQUEST_SYSTEM_LLD_HANDLERS_TEAM_LEAD_HANDLER_H

#include <iostream>

#include "LeaveApprovalHandler.h"

namespace leave_request_lld {

class TeamLeadHandler : public LeaveApprovalHandler {
protected:
    bool canApprove(const LeaveRequest& request) const override {
        return request.workingDays <= maxDays();
    }

    void approve(LeaveRequest& request) const override {
        request.status = LeaveStatus::APPROVED;
        request.approvedBy = role();
        request.remarks = "Approved by Team Lead (<= " + std::to_string(maxDays()) + " day)";
        std::cout << "  [CoR] TeamLead approved " << request.requestId << " ("
                  << request.workingDays << " day)\n";
    }

    ApproverRole role() const override { return ApproverRole::TEAM_LEAD; }
    int maxDays() const override { return 1; }
};

}  // namespace leave_request_lld

#endif  // LEAVE_REQUEST_SYSTEM_LLD_HANDLERS_TEAM_LEAD_HANDLER_H

#ifndef LEAVE_REQUEST_SYSTEM_LLD_SERVICES_LEAVE_APPROVAL_SERVICE_H
#define LEAVE_REQUEST_SYSTEM_LLD_SERVICES_LEAVE_APPROVAL_SERVICE_H

#include <iostream>
#include <stdexcept>

#include "../managers/LeaveApprovalChainManager.h"
#include "../models/LeaveRequest.h"

namespace leave_request_lld {

class LeaveApprovalService {
public:
    LeaveApprovalService() { chainManager_.buildChain(); }

    void runApprovalChain(LeaveRequest& request) {
        if (request.status != LeaveStatus::PENDING) {
            throw std::runtime_error("only PENDING requests can enter approval chain");
        }
        auto head = chainManager_.getChainHead();
        if (!head) {
            throw std::runtime_error("approval chain not initialized");
        }
        std::cout << "  [CoR] Starting chain for " << request.requestId << " ("
                  << request.workingDays << " day(s))\n";
        head->process(request);
        if (request.status == LeaveStatus::REJECTED) {
            std::cout << "  [CoR] Rejected: " << request.remarks << "\n";
        }
    }

private:
    LeaveApprovalChainManager chainManager_;
};

}  // namespace leave_request_lld

#endif  // LEAVE_REQUEST_SYSTEM_LLD_SERVICES_LEAVE_APPROVAL_SERVICE_H

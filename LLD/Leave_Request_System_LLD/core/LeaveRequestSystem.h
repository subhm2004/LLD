#ifndef LEAVE_REQUEST_SYSTEM_LLD_CORE_LEAVE_REQUEST_SYSTEM_H
#define LEAVE_REQUEST_SYSTEM_LLD_CORE_LEAVE_REQUEST_SYSTEM_H

#include <stdexcept>
#include <string>
#include <vector>

#include "../enums/LeaveStatus.h"
#include "../models/Employee.h"
#include "../models/LeaveRequest.h"
#include "../services/LeaveApprovalService.h"
#include "../services/LeaveRegistryService.h"

namespace leave_request_lld {

class LeaveRequestSystem {
public:
    std::string registerEmployee(const std::string& name, const std::string& team) {
        const std::string employeeId = "EMP_" + std::to_string(++employeeCounter_);
        registry_.registerEmployee(Employee{employeeId, name, team});
        return employeeId;
    }

    std::string submitLeave(const std::string& employeeId, LeaveType leaveType,
                            const std::string& startDate, const std::string& endDate,
                            int workingDays) {
        if (workingDays <= 0) {
            throw std::invalid_argument("workingDays must be positive");
        }
        registry_.getEmployee(employeeId);  // validate exists

        const std::string requestId = "LR_" + std::to_string(++requestCounter_);
        LeaveRequest request{requestId,     employeeId, leaveType, startDate,
                             endDate,       workingDays, LeaveStatus::PENDING,
                             ApproverRole::NONE, ""};

        registry_.saveRequest(request);
        return requestId;
    }

    void processApproval(const std::string& requestId) {
        LeaveRequest& request = registry_.getRequest(requestId);
        approvalService_.runApprovalChain(request);
    }

    void cancelLeave(const std::string& requestId) {
        LeaveRequest& request = registry_.getRequest(requestId);
        if (request.status != LeaveStatus::PENDING) {
            throw std::runtime_error("only PENDING leave can be cancelled");
        }
        request.status = LeaveStatus::CANCELLED;
        request.remarks = "Cancelled by employee";
    }

    LeaveStatus getStatus(const std::string& requestId) const {
        return registry_.getRequest(requestId).status;
    }

    LeaveRequest getLeaveRequest(const std::string& requestId) const {
        return registry_.getRequest(requestId);
    }

    std::vector<LeaveRequest> listEmployeeLeaves(const std::string& employeeId) const {
        return registry_.listByEmployee(employeeId);
    }

private:
    LeaveRegistryService registry_;
    LeaveApprovalService approvalService_;
    int employeeCounter_{0};
    int requestCounter_{0};
};

}  // namespace leave_request_lld

#endif  // LEAVE_REQUEST_SYSTEM_LLD_CORE_LEAVE_REQUEST_SYSTEM_H

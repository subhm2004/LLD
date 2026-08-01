#ifndef LEAVE_REQUEST_SYSTEM_LLD_HANDLERS_LEAVE_APPROVAL_HANDLER_H
#define LEAVE_REQUEST_SYSTEM_LLD_HANDLERS_LEAVE_APPROVAL_HANDLER_H

#include <memory>

#include "../models/LeaveRequest.h"

namespace leave_request_lld {

// Abstract handler — Chain of Responsibility base
class LeaveApprovalHandler {
public:
    virtual ~LeaveApprovalHandler() = default;

    void setNext(std::shared_ptr<LeaveApprovalHandler> nextHandler) {
        next_ = std::move(nextHandler);
    }

    void process(LeaveRequest& request) {
        if (canApprove(request)) {
            approve(request);
            return;
        }
        if (next_) {
            next_->process(request);
        } else {
            reject(request, "No approver in chain has authority for " +
                                std::to_string(request.workingDays) + " day(s)");
        }
    }

protected:
    virtual bool canApprove(const LeaveRequest& request) const = 0;
    virtual void approve(LeaveRequest& request) const = 0;
    virtual ApproverRole role() const = 0;
    virtual int maxDays() const = 0;

    void reject(LeaveRequest& request, const std::string& reason) const {
        request.status = LeaveStatus::REJECTED;
        request.approvedBy = ApproverRole::NONE;
        request.remarks = reason;
    }

private:
    std::shared_ptr<LeaveApprovalHandler> next_;
};

}  // namespace leave_request_lld

#endif  // LEAVE_REQUEST_SYSTEM_LLD_HANDLERS_LEAVE_APPROVAL_HANDLER_H

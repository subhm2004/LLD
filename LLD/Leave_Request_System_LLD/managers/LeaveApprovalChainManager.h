#ifndef LEAVE_REQUEST_SYSTEM_LLD_MANAGERS_LEAVE_APPROVAL_CHAIN_MANAGER_H
#define LEAVE_REQUEST_SYSTEM_LLD_MANAGERS_LEAVE_APPROVAL_CHAIN_MANAGER_H

#include <memory>

#include "../handlers/DirectorHandler.h"
#include "../handlers/HRHandler.h"
#include "../handlers/LeaveApprovalHandler.h"
#include "../handlers/ManagerHandler.h"
#include "../handlers/TeamLeadHandler.h"

namespace leave_request_lld {

// Builds: TeamLead → Manager → HR → Director
class LeaveApprovalChainManager {
public:
    std::shared_ptr<LeaveApprovalHandler> buildChain() {
        auto teamLead = std::make_shared<TeamLeadHandler>();
        auto manager = std::make_shared<ManagerHandler>();
        auto hr = std::make_shared<HRHandler>();
        auto director = std::make_shared<DirectorHandler>();

        teamLead->setNext(manager);
        manager->setNext(hr);
        hr->setNext(director);

        chainHead_ = teamLead;
        return chainHead_;
    }

    std::shared_ptr<LeaveApprovalHandler> getChainHead() const { return chainHead_; }

private:
    std::shared_ptr<LeaveApprovalHandler> chainHead_;
};

}  // namespace leave_request_lld

#endif  // LEAVE_REQUEST_SYSTEM_LLD_MANAGERS_LEAVE_APPROVAL_CHAIN_MANAGER_H

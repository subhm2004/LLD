#ifndef LEAVE_REQUEST_SYSTEM_LLD_SERVICES_LEAVE_REGISTRY_SERVICE_H
#define LEAVE_REQUEST_SYSTEM_LLD_SERVICES_LEAVE_REGISTRY_SERVICE_H

#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "../models/Employee.h"
#include "../models/LeaveRequest.h"

namespace leave_request_lld {

class LeaveRegistryService {
public:
    void registerEmployee(const Employee& employee) {
        employees_[employee.id] = employee;
    }

    const Employee& getEmployee(const std::string& employeeId) const {
        auto it = employees_.find(employeeId);
        if (it == employees_.end()) {
            throw std::runtime_error("employee not found: " + employeeId);
        }
        return it->second;
    }

    void saveRequest(const LeaveRequest& request) { requests_[request.requestId] = request; }

    LeaveRequest& getRequest(const std::string& requestId) {
        auto it = requests_.find(requestId);
        if (it == requests_.end()) {
            throw std::runtime_error("leave request not found: " + requestId);
        }
        return it->second;
    }

    const LeaveRequest& getRequest(const std::string& requestId) const {
        return const_cast<LeaveRegistryService*>(this)->getRequest(requestId);
    }

    std::vector<LeaveRequest> listByEmployee(const std::string& employeeId) const {
        std::vector<LeaveRequest> result;
        for (const auto& [id, req] : requests_) {
            (void)id;
            if (req.employeeId == employeeId) {
                result.push_back(req);
            }
        }
        return result;
    }

private:
    std::unordered_map<std::string, Employee> employees_;
    std::unordered_map<std::string, LeaveRequest> requests_;
};

}  // namespace leave_request_lld

#endif  // LEAVE_REQUEST_SYSTEM_LLD_SERVICES_LEAVE_REGISTRY_SERVICE_H

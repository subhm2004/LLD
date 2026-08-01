#ifndef LEAVE_REQUEST_SYSTEM_LLD_MODELS_EMPLOYEE_H
#define LEAVE_REQUEST_SYSTEM_LLD_MODELS_EMPLOYEE_H

#include <string>

namespace leave_request_lld {

struct Employee {
    std::string id;
    std::string name;
    std::string team;
};

}  // namespace leave_request_lld

#endif  // LEAVE_REQUEST_SYSTEM_LLD_MODELS_EMPLOYEE_H

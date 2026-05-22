#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_USER_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_USER_H

#include <string>

#include "../enums/UserRole.h"

namespace airline_mgmt {

struct User {
    std::string userId;
    std::string name;
    std::string email;
    UserRole role;
    std::string linkedPassengerId;
};

} // namespace airline_mgmt

#endif

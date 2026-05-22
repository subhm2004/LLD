#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_CREWMEMBER_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_CREWMEMBER_H

#include <string>

#include "../enums/CrewRole.h"

namespace airline_mgmt {

struct CrewMember {
    std::string crewId;
    std::string name;
    CrewRole role;
};

} // namespace airline_mgmt

#endif

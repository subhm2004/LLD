#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_AIRPORT_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_AIRPORT_H

#include <string>

namespace airline_mgmt {

struct Airport {
    std::string code;
    std::string name;
    std::string city;
};

} // namespace airline_mgmt

#endif

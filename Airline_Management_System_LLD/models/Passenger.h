#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_PASSENGER_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_PASSENGER_H

#include <string>

namespace airline_mgmt {

struct Passenger {
    std::string passengerId;
    std::string name;
    std::string email;
    std::string phone;
    double baggageKg;
};

} // namespace airline_mgmt

#endif

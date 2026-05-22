#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_AIRCRAFT_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_AIRCRAFT_H

#include <string>

namespace airline_mgmt {

struct Aircraft {
    std::string aircraftId;
    std::string model;
    int economySeats;
    int premiumEconomySeats;
    int businessSeats;

    int totalSeats() const { return economySeats + premiumEconomySeats + businessSeats; }
};

} // namespace airline_mgmt

#endif

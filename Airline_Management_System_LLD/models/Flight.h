#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_FLIGHT_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_MODELS_FLIGHT_H

#include <string>

#include "../enums/FlightStatus.h"

namespace airline_mgmt {

struct Flight {
    std::string flightId;
    std::string sourceAirport;
    std::string destinationAirport;
    int departureDay;
    int departureTimeMinutes;
    std::string aircraftId;
    FlightStatus status;
};

} // namespace airline_mgmt

#endif

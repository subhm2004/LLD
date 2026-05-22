#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_SERVICES_FLIGHTSEARCHSERVICE_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_SERVICES_FLIGHTSEARCHSERVICE_H

#include <string>
#include <unordered_map>
#include <vector>

#include "../enums/FlightStatus.h"
#include "../models/Flight.h"

namespace airline_mgmt {

class FlightSearchService {
public:
    static std::vector<const Flight *> search(const std::unordered_map<std::string, Flight> &flights,
                                              const std::string &source, const std::string &destination,
                                              int departureDay) {
        std::vector<const Flight *> result;
        for (const auto &entry : flights) {
            const Flight &f = entry.second;
            if (f.sourceAirport == source && f.destinationAirport == destination && f.departureDay == departureDay &&
                f.status == FlightStatus::SCHEDULED) {
                result.push_back(&entry.second);
            }
        }
        return result;
    }
};

} // namespace airline_mgmt

#endif

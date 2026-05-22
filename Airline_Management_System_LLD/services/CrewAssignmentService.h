#ifndef AIRLINE_MANAGEMENT_SYSTEM_LLD_SERVICES_CREWASSIGNMENTSERVICE_H
#define AIRLINE_MANAGEMENT_SYSTEM_LLD_SERVICES_CREWASSIGNMENTSERVICE_H

#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../enums/CrewRole.h"
#include "../models/CrewMember.h"
#include "../models/Flight.h"

namespace airline_mgmt {

class CrewAssignmentService {
public:
    static void assignCrew(const std::string &flightId, const std::string &crewId,
                           const std::unordered_map<std::string, Flight> &flights,
                           const std::unordered_map<std::string, CrewMember> &crew,
                           std::unordered_map<std::string, std::vector<std::string>> &flightCrew) {
        if (!flights.count(flightId)) {
            throw std::runtime_error("Flight not found: " + flightId);
        }
        if (!crew.count(crewId)) {
            throw std::runtime_error("Crew not found: " + crewId);
        }

        std::vector<std::string> &assigned = flightCrew[flightId];
        for (const std::string &existing : assigned) {
            if (existing == crewId) {
                throw std::runtime_error("Crew already assigned to flight");
            }
        }
        assigned.push_back(crewId);
    }

    static bool hasMinimumCrew(const std::string &flightId,
                               const std::unordered_map<std::string, std::vector<std::string>> &flightCrew,
                               const std::unordered_map<std::string, CrewMember> &crew) {
        auto it = flightCrew.find(flightId);
        if (it == flightCrew.end()) {
            return false;
        }
        bool hasPilot = false;
        bool hasCabin = false;
        for (const std::string &crewId : it->second) {
            auto crewIt = crew.find(crewId);
            if (crewIt == crew.end()) {
                continue;
            }
            if (crewIt->second.role == CrewRole::PILOT || crewIt->second.role == CrewRole::CO_PILOT) {
                hasPilot = true;
            }
            if (crewIt->second.role == CrewRole::CABIN_CREW) {
                hasCabin = true;
            }
        }
        return hasPilot && hasCabin;
    }
};

} // namespace airline_mgmt

#endif

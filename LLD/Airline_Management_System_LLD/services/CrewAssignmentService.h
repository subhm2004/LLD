// ============================================================================
//  services/CrewAssignmentService.h  —  Flight ko crew assign karo (Requirement #3)
// ----------------------------------------------------------------------------
//  Do kaam:
//    1. assignCrew()     -> ek crew member ko ek flight pe daalo (validations ke saath)
//    2. hasMinimumCrew() -> flight ke paas booking khoolne layak crew hai ya nahi
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ MINIMUM CREW RULE — booking ka ek zaroori "gate"                      │
//  │                                                                          │
//  │  Flight tabhi book ho sakti hai jab uske paas kam se kam:                 │
//  │     - ek COCKPIT wala (PILOT ya CO_PILOT), AUR                            │
//  │     - ek CABIN_CREW                                                       │
//  │  ho. Bina crew ke flight ud nahi sakti, to booking bhi nahi khulti.       │
//  │  Facade ka createBooking() booking se pehle hasMinimumCrew() check karta. │
//  │                                                                          │
//  │  💡 PILOT aur CO_PILOT dono "hasPilot" gine jaate hain — yaani ek         │
//  │     CO_PILOT bhi cockpit requirement poori kar deta (simplified rule).   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Ye service khud crew ka data OWN nahi karti — facade ke maps (flights,
//     crew, flightCrew) reference se leti hai aur unpe kaam karti hai. Stateless
//     helper design (static methods). `flightCrew` = flightId -> [crewId...] mapping.
// ============================================================================
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
    // ---- Ek crew member ko flight pe assign karo ---------------------------
    static void assignCrew(const std::string &flightId, const std::string &crewId,
                           const std::unordered_map<std::string, Flight> &flights,
                           const std::unordered_map<std::string, CrewMember> &crew,
                           std::unordered_map<std::string, std::vector<std::string>> &flightCrew) {
        // Validation 1 & 2: flight aur crew dono exist karte hain?
        if (!flights.count(flightId)) {
            throw std::runtime_error("Flight not found: " + flightId);
        }
        if (!crew.count(crewId)) {
            throw std::runtime_error("Crew not found: " + crewId);
        }

        // Validation 3: same crew do baar assign na ho (duplicate rok)
        std::vector<std::string> &assigned = flightCrew[flightId];
        for (const std::string &existing : assigned) {
            if (existing == crewId) {
                throw std::runtime_error("Crew already assigned to flight");
            }
        }
        assigned.push_back(crewId);
    }

    // ---- Flight ke paas minimum crew hai? (cockpit + cabin) ----------------
    static bool hasMinimumCrew(const std::string &flightId,
                               const std::unordered_map<std::string, std::vector<std::string>> &flightCrew,
                               const std::unordered_map<std::string, CrewMember> &crew) {
        auto it = flightCrew.find(flightId);
        if (it == flightCrew.end()) {
            return false; // is flight pe koi crew hi nahi
        }
        bool hasPilot = false; // cockpit wala (PILOT ya CO_PILOT)
        bool hasCabin = false; // cabin crew
        for (const std::string &crewId : it->second) {
            auto crewIt = crew.find(crewId);
            if (crewIt == crew.end()) {
                continue; // crew record hi missing (defensive skip)
            }
            if (crewIt->second.role == CrewRole::PILOT || crewIt->second.role == CrewRole::CO_PILOT) {
                hasPilot = true;
            }
            if (crewIt->second.role == CrewRole::CABIN_CREW) {
                hasCabin = true;
            }
        }
        return hasPilot && hasCabin; // dono chahiye tabhi true
    }
};

} // namespace airline_mgmt

#endif

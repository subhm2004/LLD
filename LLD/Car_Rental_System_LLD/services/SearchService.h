// ============================================================================
//  services/SearchService.h — Vehicle SEARCH (requirement #11) [NAYA]
// ----------------------------------------------------------------------------
//  Requirement #11: users vehicles ko TYPE ya MODEL se search kar sakein.
//  Ye service saare branches ki inventory pe search chalata (cross-branch).
//
//  Do search methods:
//    searchByType(type)   -> us type ke saare available vehicles (car/van...)
//    searchByModel(query) -> model naam me substring match (case-insensitive)
//
//  Sirf AVAILABLE vehicles return karta (jo rent ho sakti). Business logic
//  models se alag (separation of concerns) — search ka apna service.
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_SERVICES_SEARCHSERVICE_H
#define CAR_RENTAL_SYSTEM_LLD_SERVICES_SEARCHSERVICE_H

#include <bits/stdc++.h>

#include "../enums/VehicleStatus.h"
#include "../enums/VehicleType.h"
#include "../models/Branch.h"
#include "../models/Vehicle.h"

using namespace std;

namespace car_rental_lld {

class SearchService {
public:
    // Requirement #11 — type se search (saare branches me)
    static vector<Vehicle *> searchByType(const vector<Branch *> &branches, VehicleType type) {
        vector<Vehicle *> results;
        for (Branch *branch : branches) {
            for (Vehicle *vehicle : branch->getVehicles()) {
                if (vehicle->getType() == type && vehicle->getStatus() == VehicleStatus::AVAILABLE) {
                    results.push_back(vehicle);
                }
            }
        }
        return results;
    }

    // Requirement #11 — model se search (substring, case-insensitive)
    static vector<Vehicle *> searchByModel(const vector<Branch *> &branches, const string &query) {
        vector<Vehicle *> results;
        const string lowerQuery = toLower(query);
        for (Branch *branch : branches) {
            for (Vehicle *vehicle : branch->getVehicles()) {
                if (vehicle->getStatus() != VehicleStatus::AVAILABLE) {
                    continue;
                }
                if (toLower(vehicle->getModelName()).find(lowerQuery) != string::npos) {
                    results.push_back(vehicle);
                }
            }
        }
        return results;
    }

private:
    static string toLower(const string &s) {
        string out = s;
        transform(out.begin(), out.end(), out.begin(), ::tolower);
        return out;
    }
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_SERVICES_SEARCHSERVICE_H

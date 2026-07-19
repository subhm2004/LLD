// ============================================================================
//  models/Motorcycle.h — Concrete Vehicle: MOTORCYCLE + subtypes (req #3) [NAYA]
// ----------------------------------------------------------------------------
//  Requirement #3: Motorcycles — CRUISER / TOURING / SPORTS.
//  MotorcycleType enum + getSubtype() override + fromString (factory ke liye).
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_MODELS_MOTORCYCLE_H
#define CAR_RENTAL_SYSTEM_LLD_MODELS_MOTORCYCLE_H

#include <bits/stdc++.h>

#include "Vehicle.h"

using namespace std;

namespace car_rental_lld {

enum class MotorcycleType { CRUISER, TOURING, SPORTS };

class Motorcycle : public Vehicle {
public:
    Motorcycle(string id, string model, MotorcycleType subtype, double dailyRate)
        : Vehicle(std::move(id), std::move(model), VehicleType::MOTORCYCLE, dailyRate), subtype_(subtype) {}

    string getSubtype() const override {
        switch (subtype_) {
            case MotorcycleType::CRUISER: return "CRUISER";
            case MotorcycleType::TOURING: return "TOURING";
            case MotorcycleType::SPORTS: return "SPORTS";
        }
        return "UNKNOWN";
    }

    static MotorcycleType fromString(const string &s) {
        if (s == "TOURING") return MotorcycleType::TOURING;
        if (s == "SPORTS") return MotorcycleType::SPORTS;
        return MotorcycleType::CRUISER;
    }

private:
    MotorcycleType subtype_;
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_MODELS_MOTORCYCLE_H

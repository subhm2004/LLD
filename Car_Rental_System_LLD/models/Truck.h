// ============================================================================
//  models/Truck.h — Concrete Vehicle: TRUCK + subtypes (requirement #3) [NAYA]
// ----------------------------------------------------------------------------
//  Requirement #3: Trucks — LIGHT / MEDIUM / HIGH_DUTY (load capacity ke hisaab).
//  TruckType enum + getSubtype() override + fromString.
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_MODELS_TRUCK_H
#define CAR_RENTAL_SYSTEM_LLD_MODELS_TRUCK_H

#include <bits/stdc++.h>

#include "Vehicle.h"

using namespace std;

namespace car_rental_lld {

enum class TruckType { LIGHT, MEDIUM, HIGH_DUTY };

class Truck : public Vehicle {
public:
    Truck(string id, string model, TruckType subtype, double dailyRate)
        : Vehicle(std::move(id), std::move(model), VehicleType::TRUCK, dailyRate), subtype_(subtype) {}

    string getSubtype() const override {
        switch (subtype_) {
            case TruckType::LIGHT: return "LIGHT";
            case TruckType::MEDIUM: return "MEDIUM";
            case TruckType::HIGH_DUTY: return "HIGH_DUTY";
        }
        return "UNKNOWN";
    }

    static TruckType fromString(const string &s) {
        if (s == "MEDIUM") return TruckType::MEDIUM;
        if (s == "HIGH_DUTY") return TruckType::HIGH_DUTY;
        return TruckType::LIGHT;
    }

private:
    TruckType subtype_;
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_MODELS_TRUCK_H

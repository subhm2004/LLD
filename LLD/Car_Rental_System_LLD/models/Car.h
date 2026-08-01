// ============================================================================
//  models/Car.h — Concrete Vehicle: CAR + subtypes (requirement #3) [NAYA]
// ----------------------------------------------------------------------------
//  Requirement #3: Cars ki subcategories — ECONOMY / LUXURY / STANDARD / COMPACT.
//  CarType enum yahin define hai (subtype apni vehicle ke saath rehta = cohesion).
//  getSubtype() override karke Car apna subtype string deta.
//
//  fromString() helper — VehicleFactory string ("LUXURY") se enum banata.
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_MODELS_CAR_H
#define CAR_RENTAL_SYSTEM_LLD_MODELS_CAR_H

#include <bits/stdc++.h>

#include "Vehicle.h"

using namespace std;

namespace car_rental_lld {

enum class CarType { ECONOMY, LUXURY, STANDARD, COMPACT };

class Car : public Vehicle {
public:
    Car(string id, string model, CarType subtype, double dailyRate)
        : Vehicle(std::move(id), std::move(model), VehicleType::CAR, dailyRate), subtype_(subtype) {}

    string getSubtype() const override {
        switch (subtype_) {
            case CarType::ECONOMY: return "ECONOMY";
            case CarType::LUXURY: return "LUXURY";
            case CarType::STANDARD: return "STANDARD";
            case CarType::COMPACT: return "COMPACT";
        }
        return "UNKNOWN";
    }

    // String -> enum (factory ke liye)
    static CarType fromString(const string &s) {
        if (s == "LUXURY") return CarType::LUXURY;
        if (s == "STANDARD") return CarType::STANDARD;
        if (s == "COMPACT") return CarType::COMPACT;
        return CarType::ECONOMY;  // default
    }

private:
    CarType subtype_;
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_MODELS_CAR_H

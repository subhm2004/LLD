// ============================================================================
//  models/Van.h — Concrete Vehicle: VAN + subtypes (requirement #3) [NAYA]
// ----------------------------------------------------------------------------
//  Requirement #3: Vans — PASSENGER (log dhone wali) ya CARGO (saaman dhone wali).
//  Car.h jaisa hi structure — VanType enum + getSubtype() override + fromString.
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_MODELS_VAN_H
#define CAR_RENTAL_SYSTEM_LLD_MODELS_VAN_H

#include <bits/stdc++.h>

#include "Vehicle.h"

using namespace std;

namespace car_rental_lld {

enum class VanType { PASSENGER, CARGO };

class Van : public Vehicle {
public:
    Van(string id, string model, VanType subtype, double dailyRate)
        : Vehicle(std::move(id), std::move(model), VehicleType::VAN, dailyRate), subtype_(subtype) {}

    string getSubtype() const override {
        return subtype_ == VanType::PASSENGER ? "PASSENGER" : "CARGO";
    }

    static VanType fromString(const string &s) {
        return s == "CARGO" ? VanType::CARGO : VanType::PASSENGER;
    }

private:
    VanType subtype_;
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_MODELS_VAN_H

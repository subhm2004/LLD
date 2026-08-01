// ============================================================================
//  models/Vehicle.h — ABSTRACT base Vehicle (requirement #2/#3 ka foundation)
// ----------------------------------------------------------------------------
//  Pehle Vehicle ek simple concrete class thi (id + model + rate). Ab
//  requirement #2/#3 ke hisaab se ye ABSTRACT BASE hai — Car/Van/Motorcycle/
//  Truck isse extend karte hain, har ek apne SUBTYPE ke saath.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │                    Vehicle (abstract base)                              │
//  │        id, model, type, dailyRate, status + getSubtype()*               │
//  │                         △                                                │
//  │       ┌───────────┬─────┴─────┬────────────┐                            │
//  │     Car          Van      Motorcycle      Truck                         │
//  │  economy/       passenger/  cruiser/    light/                          │
//  │  luxury/         cargo      touring/     medium/                        │
//  │  standard/                  sports       high-duty                      │
//  │  compact                                                                │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  getSubtype() PURE VIRTUAL hai — har concrete vehicle apna subtype string
//  deta ("ECONOMY", "CARGO"...). Isse polymorphism milta: system sirf
//  Vehicle* jaanta hai, subtype virtual dispatch se aata.
//
//  Ownership: vehicles ab POINTER se store hote (polymorphism ke liye).
//  Branch inventory inhe own karta.
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_MODELS_VEHICLE_H
#define CAR_RENTAL_SYSTEM_LLD_MODELS_VEHICLE_H

#include <bits/stdc++.h>

#include "../enums/VehicleStatus.h"
#include "../enums/VehicleType.h"

using namespace std;

namespace car_rental_lld {

class Vehicle {
public:
    Vehicle(string id, string model, VehicleType type, double dailyRate)
        : id_(std::move(id)),
          model_(std::move(model)),
          type_(type),
          dailyRate_(dailyRate),
          status_(VehicleStatus::AVAILABLE) {}

    virtual ~Vehicle() = default;

    const string &getId() const { return id_; }
    const string &getModelName() const { return model_; }
    VehicleType getType() const { return type_; }
    double getDailyRate() const { return dailyRate_; }
    VehicleStatus getStatus() const { return status_; }
    void setStatus(VehicleStatus status) { status_ = status; }

    // Har concrete vehicle apna subtype batata (requirement #3)
    virtual string getSubtype() const = 0;

    // Display helper — "CAR (LUXURY)" jaisa
    string describe() const {
        return vehicleTypeToString(type_) + " (" + getSubtype() + ") " + model_;
    }

protected:
    string id_;
    string model_;
    VehicleType type_;
    double dailyRate_;
    VehicleStatus status_;
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_MODELS_VEHICLE_H

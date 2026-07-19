// ============================================================================
//  factories/VehicleFactory.h — FACTORY (L9): sahi vehicle banane ki dukaan [NAYA]
// ----------------------------------------------------------------------------
//  VehicleType (car/van/moto/truck) + subtype string ("LUXURY"/"CARGO"...) do,
//  sahi concrete Vehicle object lo. Client ko Car/Van/Motorcycle/Truck concrete
//  classes ka pata nahi — bas type+subtype bolo, factory banati hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │   createVehicle(CAR, "LUXURY", id, model, rate)                         │
//  │        -> new Car(id, model, Car::fromString("LUXURY"), rate)           │
//  │   createVehicle(VAN, "CARGO", ...)  -> new Van(...)                     │
//  │   createVehicle(MOTORCYCLE, "SPORTS", ...) -> new Motorcycle(...)       │
//  │   createVehicle(TRUCK, "HIGH_DUTY", ...)   -> new Truck(...)            │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ FAYDA: naya vehicle type add karna ho (jaise "Bus") to bas ek case +
//  nayi Vehicle subclass — baaki system (branch, booking, search) untouched.
//  Creation logic ek jagah centralized. Return Vehicle* (base pointer,
//  polymorphism) — caller/branch ownership sambhale.
// ============================================================================
#ifndef CAR_RENTAL_SYSTEM_LLD_FACTORIES_VEHICLEFACTORY_H
#define CAR_RENTAL_SYSTEM_LLD_FACTORIES_VEHICLEFACTORY_H

#include <bits/stdc++.h>

#include "../enums/VehicleType.h"
#include "../models/Car.h"
#include "../models/Motorcycle.h"
#include "../models/Truck.h"
#include "../models/Van.h"
#include "../models/Vehicle.h"

using namespace std;

namespace car_rental_lld {

class VehicleFactory {
public:
    static Vehicle *createVehicle(VehicleType type, const string &subtype, const string &id,
                                  const string &model, double dailyRate) {
        switch (type) {
            case VehicleType::CAR:
                return new Car(id, model, Car::fromString(subtype), dailyRate);
            case VehicleType::VAN:
                return new Van(id, model, Van::fromString(subtype), dailyRate);
            case VehicleType::MOTORCYCLE:
                return new Motorcycle(id, model, Motorcycle::fromString(subtype), dailyRate);
            case VehicleType::TRUCK:
                return new Truck(id, model, Truck::fromString(subtype), dailyRate);
        }
        throw runtime_error("Unknown vehicle type");
    }
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_FACTORIES_VEHICLEFACTORY_H

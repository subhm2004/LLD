#ifndef CAR_RENTAL_SYSTEM_LLD_MODELS_VEHICLE_H
#define CAR_RENTAL_SYSTEM_LLD_MODELS_VEHICLE_H

#include <bits/stdc++.h>

#include "../enums/VehicleStatus.h"

using namespace std;

namespace car_rental_lld {

class Vehicle {
public:
    Vehicle(string vehicleId, string modelName, double dailyRate)
        : vehicleId_(std::move(vehicleId)),
          modelName_(std::move(modelName)),
          dailyRate_(dailyRate),
          status_(VehicleStatus::AVAILABLE) {}

    const string &getId() const { return vehicleId_; }
    const string &getModelName() const { return modelName_; }
    double getDailyRate() const { return dailyRate_; }
    VehicleStatus getStatus() const { return status_; }
    void setStatus(VehicleStatus status) { status_ = status; }

private:
    string vehicleId_;
    string modelName_;
    double dailyRate_;
    VehicleStatus status_;
};

} // namespace car_rental_lld

#endif // CAR_RENTAL_SYSTEM_LLD_MODELS_VEHICLE_H

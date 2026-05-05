#ifndef UBER_LLD_MODELS_DRIVER_H
#define UBER_LLD_MODELS_DRIVER_H

#include <bits/stdc++.h>

#include "../enums/DriverStatus.h"
#include "Location.h"

using namespace std;

namespace uber_lld {

class Driver {
public:
    Driver(string driverId, string name, string vehicleNumber, Location location)
        : driverId_(std::move(driverId)),
          name_(std::move(name)),
          vehicleNumber_(std::move(vehicleNumber)),
          location_(location),
          status_(DriverStatus::AVAILABLE) {}

    const string &getId() const { return driverId_; }
    const string &getName() const { return name_; }
    const string &getVehicleNumber() const { return vehicleNumber_; }
    Location getLocation() const { return location_; }
    DriverStatus getStatus() const { return status_; }

    void setLocation(Location location) { location_ = location; }
    void setStatus(DriverStatus status) { status_ = status; }

private:
    string driverId_;
    string name_;
    string vehicleNumber_;
    Location location_;
    DriverStatus status_;
};

} // namespace uber_lld

#endif // UBER_LLD_MODELS_DRIVER_H

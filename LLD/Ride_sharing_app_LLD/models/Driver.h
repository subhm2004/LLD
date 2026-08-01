#ifndef RIDE_SHARING_APP_LLD_MODELS_DRIVER_H
#define RIDE_SHARING_APP_LLD_MODELS_DRIVER_H

#include <string>
#include <utility>

#include "../enums/DriverStatus.h"
#include "Location.h"

using namespace std;

namespace ride_sharing {

class Driver {
public:
    Driver(string driverId, string name, Location location)
        : driverId_(std::move(driverId)),
          name_(std::move(name)),
          currentLocation_(location),
          status_(DriverStatus::AVAILABLE) {}

    const string &getId() const { return driverId_; }
    const string &getName() const { return name_; }
    Location getLocation() const { return currentLocation_; }
    DriverStatus getStatus() const { return status_; }

    void setLocation(Location location) { currentLocation_ = location; }
    void setStatus(DriverStatus status) { status_ = status; }

private:
    string driverId_;
    string name_;
    Location currentLocation_;
    DriverStatus status_;
};

} // namespace ride_sharing

#endif // RIDE_SHARING_APP_LLD_MODELS_DRIVER_H

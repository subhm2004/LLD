#ifndef RIDE_SHARING_APP_LLD_MODELS_RIDE_H
#define RIDE_SHARING_APP_LLD_MODELS_RIDE_H

#include <ctime>
#include <string>
#include <utility>

#include "../enums/RideStatus.h"
#include "Location.h"

using namespace std;

namespace ride_sharing {

class Ride {
public:
    Ride(string rideId,
         string riderId,
         string driverId,
         Location pickup,
         Location drop,
         double fare)
        : rideId_(std::move(rideId)),
          riderId_(std::move(riderId)),
          driverId_(std::move(driverId)),
          pickup_(pickup),
          drop_(drop),
          fare_(fare),
          status_(RideStatus::REQUESTED),
          createdAt_(time(nullptr)) {}

    const string &getId() const { return rideId_; }
    const string &getRiderId() const { return riderId_; }
    const string &getDriverId() const { return driverId_; }
    RideStatus getStatus() const { return status_; }
    double getFare() const { return fare_; }
    Location getDrop() const { return drop_; }

    void setStatus(RideStatus status) { status_ = status; }

private:
    string rideId_;
    string riderId_;
    string driverId_;
    Location pickup_;
    Location drop_;
    double fare_;
    RideStatus status_;
    long long createdAt_;
};

} // namespace ride_sharing

#endif // RIDE_SHARING_APP_LLD_MODELS_RIDE_H

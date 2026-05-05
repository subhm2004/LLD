#ifndef RIDE_SHARING_APP_LLD_CORE_RIDESHARINGSERVICE_H
#define RIDE_SHARING_APP_LLD_CORE_RIDESHARINGSERVICE_H

#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "../enums/DriverStatus.h"
#include "../enums/RideStatus.h"
#include "../models/Driver.h"
#include "../models/Ride.h"
#include "../models/Rider.h"
#include "../services/PricingService.h"
#include "../utils/GeoUtils.h"

using namespace std;

namespace ride_sharing {

class RideSharingService {
public:
    RideSharingService() : rideCounter_(0) {}

    ~RideSharingService() {
        for (auto &kv : riders_) {
            delete kv.second;
        }
        for (auto &kv : drivers_) {
            delete kv.second;
        }
        for (auto &kv : rides_) {
            delete kv.second;
        }
    }

    void registerRider(const string &riderId, const string &name) {
        if (riders_.count(riderId)) {
            throw runtime_error("Rider already exists");
        }
        riders_[riderId] = new Rider(riderId, name);
    }

    void registerDriver(const string &driverId, const string &name, Location location) {
        if (drivers_.count(driverId)) {
            throw runtime_error("Driver already exists");
        }
        drivers_[driverId] = new Driver(driverId, name, location);
    }

    string requestRide(const string &riderId, Location pickup, Location drop) {
        getRiderOrThrow(riderId);
        Driver *driver = findNearestAvailableDriver(pickup);
        if (!driver) {
            throw runtime_error("No available drivers nearby");
        }

        const double fare = pricing_.calculateFare(pickup, drop);
        const string rideId = "RIDE_" + to_string(++rideCounter_);
        Ride *ride = new Ride(rideId, riderId, driver->getId(), pickup, drop, fare);
        ride->setStatus(RideStatus::ACCEPTED);
        rides_[rideId] = ride;

        driver->setStatus(DriverStatus::ON_RIDE);
        driverToActiveRide_[driver->getId()] = rideId;

        cout << "Ride booked: " << rideId << " | Driver: " << driver->getName() << " | Fare: " << fare << "\n";
        return rideId;
    }

    void startRide(const string &rideId) {
        Ride *ride = getRideOrThrow(rideId);
        if (ride->getStatus() != RideStatus::ACCEPTED) {
            throw runtime_error("Ride cannot be started");
        }
        ride->setStatus(RideStatus::STARTED);
        cout << "Ride started: " << rideId << "\n";
    }

    void completeRide(const string &rideId) {
        Ride *ride = getRideOrThrow(rideId);
        if (ride->getStatus() != RideStatus::STARTED) {
            throw runtime_error("Ride cannot be completed");
        }

        ride->setStatus(RideStatus::COMPLETED);
        Driver *driver = getDriverOrThrow(ride->getDriverId());
        driver->setStatus(DriverStatus::AVAILABLE);
        driver->setLocation(ride->getDrop());
        driverToActiveRide_.erase(driver->getId());

        cout << "Ride completed: " << rideId << " | Total Fare: " << ride->getFare() << "\n";
    }

private:
    unordered_map<string, Rider *> riders_;
    unordered_map<string, Driver *> drivers_;
    unordered_map<string, Ride *> rides_;
    unordered_map<string, string> driverToActiveRide_;
    PricingService pricing_;
    int rideCounter_;

    Rider *getRiderOrThrow(const string &riderId) const {
        auto it = riders_.find(riderId);
        if (it == riders_.end()) {
            throw runtime_error("Rider not found");
        }
        return it->second;
    }

    Driver *getDriverOrThrow(const string &driverId) const {
        auto it = drivers_.find(driverId);
        if (it == drivers_.end()) {
            throw runtime_error("Driver not found");
        }
        return it->second;
    }

    Ride *getRideOrThrow(const string &rideId) const {
        auto it = rides_.find(rideId);
        if (it == rides_.end()) {
            throw runtime_error("Ride not found");
        }
        return it->second;
    }

    Driver *findNearestAvailableDriver(Location pickup) const {
        Driver *bestDriver = nullptr;
        double minDistance = numeric_limits<double>::max();

        for (const auto &kv : drivers_) {
            Driver *driver = kv.second;
            if (driver->getStatus() != DriverStatus::AVAILABLE) {
                continue;
            }

            const double distance = calculateDistance(driver->getLocation(), pickup);
            if (distance < minDistance) {
                minDistance = distance;
                bestDriver = driver;
            }
        }
        return bestDriver;
    }
};

} // namespace ride_sharing

#endif // RIDE_SHARING_APP_LLD_CORE_RIDESHARINGSERVICE_H

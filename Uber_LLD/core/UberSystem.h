#ifndef UBER_LLD_CORE_UBERSYSTEM_H
#define UBER_LLD_CORE_UBERSYSTEM_H

#include <bits/stdc++.h>

#include "../enums/DriverStatus.h"
#include "../enums/RideStatus.h"
#include "../models/Driver.h"
#include "../models/Payment.h"
#include "../models/Ride.h"
#include "../models/Rider.h"
#include "../services/DriverMatchingService.h"
#include "../services/FareService.h"
#include "../services/OtpService.h"
#include "../services/PaymentService.h"

using namespace std;

namespace uber_lld {

class UberSystem {
public:
    ~UberSystem() {
        for (auto &entry : riders_) {
            delete entry.second;
        }
        for (auto &entry : drivers_) {
            delete entry.second;
        }
        for (auto &entry : rides_) {
            delete entry.second;
        }
    }

    void registerRider(const string &riderId, const string &name, const string &phone) {
        if (riders_.count(riderId) != 0) {
            throw runtime_error("Rider already exists");
        }
        riders_[riderId] = new Rider(riderId, name, phone);
    }

    void registerDriver(const string &driverId, const string &name, const string &vehicleNumber, Location location) {
        if (drivers_.count(driverId) != 0) {
            throw runtime_error("Driver already exists");
        }
        drivers_[driverId] = new Driver(driverId, name, vehicleNumber, location);
    }

    string requestRide(const string &riderId, Location pickup, Location drop) {
        getRiderOrThrow(riderId);
        Driver *driver = matchingService_.findNearestAvailableDriver(drivers_, pickup);
        if (driver == nullptr) {
            throw runtime_error("No available driver nearby");
        }

        const FareDetails fareDetails = fareService_.calculateFareDetails(pickup, drop);
        const double fare = fareDetails.totalFare;
        const string rideId = "RIDE_" + to_string(++rideCounter_);
        Ride *ride = new Ride(rideId, riderId, driver->getId(), pickup, drop, fare);
        ride->setStatus(RideStatus::ACCEPTED);
        rides_[rideId] = ride;

        driver->setStatus(DriverStatus::ON_RIDE);
        driverToRide_[driver->getId()] = rideId;
        const int otp = otpService_.generateOtp(rideId);

        cout << "Ride booked: " << rideId << " | Driver: " << driver->getName()
             << " | Fare Breakdown [Base: " << fareDetails.baseFare << ", DistanceKm: " << fareDetails.distanceKm
             << ", PerKm: " << fareDetails.perKmRate << ", Total: " << fareDetails.totalFare << "]"
             << " | OTP: " << otp << "\n";
        return rideId;
    }

    void startRide(const string &rideId, int otp) {
        Ride *ride = getRideOrThrow(rideId);
        if (ride->getStatus() != RideStatus::ACCEPTED) {
            throw runtime_error("Ride cannot be started");
        }
        if (!otpService_.verifyOtp(rideId, otp)) {
            throw runtime_error("Invalid OTP. Ride cannot start");
        }
        ride->setStatus(RideStatus::STARTED);
        cout << "Ride started: " << rideId << "\n";
    }

    void completeRide(const string &rideId, PaymentMethod paymentMethod) {
        Ride *ride = getRideOrThrow(rideId);
        if (ride->getStatus() != RideStatus::STARTED) {
            throw runtime_error("Ride cannot be completed");
        }

        Payment *payment = paymentService_.createPayment(rideId, ride->getFare(), paymentMethod);
        const bool paymentSuccess = paymentService_.processPayment(payment);
        if (!paymentSuccess) {
            throw runtime_error("Payment failed. Ride completion blocked");
        }

        ride->setStatus(RideStatus::COMPLETED);
        Driver *driver = getDriverOrThrow(ride->getDriverId());
        driver->setStatus(DriverStatus::AVAILABLE);
        driver->setLocation(ride->getDrop());
        driverToRide_.erase(driver->getId());

        const FareDetails fareDetails = fareService_.calculateFareDetails(ride->getPickup(), ride->getDrop());
        cout << "Ride completed: " << rideId << " | Total Fare: " << fareDetails.totalFare
             << " (Base: " << fareDetails.baseFare << ", DistanceKm: " << fareDetails.distanceKm
             << ", PerKm: " << fareDetails.perKmRate << ")"
             << " | Payment: SUCCESS | PaymentId: " << payment->getPaymentId() << "\n";
    }

    void cancelRide(const string &rideId) {
        Ride *ride = getRideOrThrow(rideId);
        if (ride->getStatus() != RideStatus::ACCEPTED) {
            throw runtime_error("Only accepted ride can be cancelled");
        }

        ride->setStatus(RideStatus::CANCELLED);
        Driver *driver = getDriverOrThrow(ride->getDriverId());
        driver->setStatus(DriverStatus::AVAILABLE);
        driverToRide_.erase(driver->getId());
        cout << "Ride cancelled: " << rideId << "\n";
    }

private:
    unordered_map<string, Rider *> riders_;
    unordered_map<string, Driver *> drivers_;
    unordered_map<string, Ride *> rides_;
    unordered_map<string, string> driverToRide_;
    DriverMatchingService matchingService_;
    FareService fareService_;
    OtpService otpService_;
    PaymentService paymentService_;
    int rideCounter_ = 0;

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
};

} // namespace uber_lld

#endif // UBER_LLD_CORE_UBERSYSTEM_H

#ifndef AMAZON_LOCKER_SERVICE_LLD_CORE_AMAZONLOCKERSERVICE_H
#define AMAZON_LOCKER_SERVICE_LLD_CORE_AMAZONLOCKERSERVICE_H

#include <chrono>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "../enums/AccessCodeStatus.h"
#include "../enums/LockerSize.h"
#include "../enums/PackageStatus.h"
#include "../models/LockerStation.h"
#include "../models/Package.h"
#include "../services/AccessCodeService.h"
#include "../services/NotificationService.h"
#include "../strategies/FirstFitAllocationStrategy.h"
#include "../strategies/ICompartmentAllocationStrategy.h"

namespace amazon_locker_lld {

class AmazonLockerService {
public:
    AmazonLockerService()
        : allocationStrategy_(std::make_shared<FirstFitAllocationStrategy>()),
          packageCounter_(0) {}

    void registerStation(const LockerStation& station) {
        stations_.insert_or_assign(station.stationId(), station);
    }

    LockerStation& getStation(const std::string& stationId) {
        auto it = stations_.find(stationId);
        if (it == stations_.end()) {
            throw std::invalid_argument("Unknown locker station: " + stationId);
        }
        return it->second;
    }

    std::string depositPackage(const std::string& stationId, const std::string& orderId,
                               const std::string& customerId, LockerSize packageSize) {
        LockerStation& station = getStation(stationId);
        const std::optional<std::string> compartmentId =
            allocationStrategy_->allocate(station, packageSize);

        if (!compartmentId.has_value()) {
            throw std::runtime_error("No available compartment for size " +
                                     lockerSizeToString(packageSize));
        }

        const std::string packageId = "PKG_" + std::to_string(++packageCounter_);
        Package package(packageId, orderId, customerId, packageSize);
        package.markDeposited(stationId, *compartmentId);
        packages_.insert_or_assign(packageId, package);

        LockerCompartment& compartment = station.getCompartment(*compartmentId);
        compartment.occupy(packageId);

        const long long now = nowEpoch();
        AccessCode accessCode =
            accessCodeService_.generate(packageId, *compartmentId, now);
        accessCodeService_.save(accessCode);

        notificationService_.notifyPickupCode(customerId, stationId, *compartmentId,
                                              accessCode.code(),
                                              accessCodeService_.validityHours());

        lastDepositedPackageId_ = packageId;
        lastDepositedCode_ = accessCode.code();

        std::cout << "Deposited | pkg=" << packageId << " | station=" << stationId
                  << " | compartment=" << *compartmentId << " | size="
                  << lockerSizeToString(packageSize) << "\n";
        return packageId;
    }

    bool pickupPackage(const std::string& stationId, const std::string& enteredCode) {
        LockerStation& station = getStation(stationId);
        const long long now = nowEpoch();

        AccessCode& accessCode = accessCodeService_.getByCode(enteredCode);

        if (accessCode.status() == AccessCodeStatus::USED) {
            throw std::runtime_error("Pickup code already used");
        }
        if (accessCode.status() == AccessCodeStatus::LOCKED) {
            throw std::runtime_error("Pickup code locked due to failed attempts");
        }
        if (accessCode.isExpired(now)) {
            accessCode.markExpired();
            accessCodeService_.save(accessCode);
            expirePackage(accessCode.packageId());
            throw std::runtime_error("Pickup code expired");
        }

        if (accessCode.code() != enteredCode) {
            accessCode.recordFailedAttempt(now);
            accessCodeService_.save(accessCode);
            throw std::invalid_argument("Wrong pickup code");
        }

        Package& package = packages_.at(accessCode.packageId());
        if (package.stationId() != stationId) {
            throw std::invalid_argument("Code not valid for this station");
        }

        LockerCompartment& compartment = station.getCompartment(accessCode.compartmentId());
        compartment.release();
        package.markPickedUp();
        accessCode.markUsed();
        accessCodeService_.save(accessCode);

        notificationService_.notifyPickupSuccess(package.customerId(), package.packageId());
        std::cout << "Picked up | pkg=" << package.packageId() << " | compartment="
                  << accessCode.compartmentId() << "\n";
        return true;
    }

    void printStationAvailability(const std::string& stationId) const {
        const auto it = stations_.find(stationId);
        if (it == stations_.end()) {
            throw std::invalid_argument("Unknown locker station: " + stationId);
        }
        it->second.printAvailability();
    }

    const Package& getPackage(const std::string& packageId) const { return packages_.at(packageId); }

    std::string getPickupCodeForDemo(const std::string& packageId) const {
        return accessCodeService_.getCodeForPackage(packageId);
    }

    const std::string& lastDepositedCode() const { return lastDepositedCode_; }
    const std::string& lastDepositedPackageId() const { return lastDepositedPackageId_; }

private:
    long long nowEpoch() const {
        return std::chrono::duration_cast<std::chrono::seconds>(
                   std::chrono::system_clock::now().time_since_epoch())
            .count();
    }

    void expirePackage(const std::string& packageId) {
        auto it = packages_.find(packageId);
        if (it != packages_.end()) {
            it->second.markExpired();
        }
    }

    std::shared_ptr<ICompartmentAllocationStrategy> allocationStrategy_;
    AccessCodeService accessCodeService_;
    NotificationService notificationService_;
    std::unordered_map<std::string, LockerStation> stations_;
    std::unordered_map<std::string, Package> packages_;
    int packageCounter_;
    std::string lastDepositedCode_;
    std::string lastDepositedPackageId_;
};

}  // namespace amazon_locker_lld

#endif  // AMAZON_LOCKER_SERVICE_LLD_CORE_AMAZONLOCKERSERVICE_H

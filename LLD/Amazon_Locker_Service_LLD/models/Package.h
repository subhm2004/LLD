#ifndef AMAZON_LOCKER_SERVICE_LLD_MODELS_PACKAGE_H
#define AMAZON_LOCKER_SERVICE_LLD_MODELS_PACKAGE_H

#include <string>

#include "../enums/LockerSize.h"
#include "../enums/PackageStatus.h"

namespace amazon_locker_lld {

class Package {
public:
    Package(std::string packageId, std::string orderId, std::string customerId, LockerSize size)
        : packageId_(std::move(packageId)),
          orderId_(std::move(orderId)),
          customerId_(std::move(customerId)),
          size_(size),
          status_(PackageStatus::CREATED),
          compartmentId_(""),
          stationId_("") {}

    const std::string& packageId() const { return packageId_; }
    const std::string& orderId() const { return orderId_; }
    const std::string& customerId() const { return customerId_; }
    LockerSize size() const { return size_; }
    PackageStatus status() const { return status_; }
    const std::string& compartmentId() const { return compartmentId_; }
    const std::string& stationId() const { return stationId_; }

    void markDeposited(const std::string& stationId, const std::string& compartmentId) {
        stationId_ = stationId;
        compartmentId_ = compartmentId;
        status_ = PackageStatus::DEPOSITED;
    }

    void markPickedUp() { status_ = PackageStatus::PICKED_UP; }
    void markExpired() { status_ = PackageStatus::EXPIRED; }

private:
    std::string packageId_;
    std::string orderId_;
    std::string customerId_;
    LockerSize size_;
    PackageStatus status_;
    std::string compartmentId_;
    std::string stationId_;
};

}  // namespace amazon_locker_lld

#endif  // AMAZON_LOCKER_SERVICE_LLD_MODELS_PACKAGE_H

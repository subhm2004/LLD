#ifndef AMAZON_LOCKER_SERVICE_LLD_MODELS_LOCKERCOMPARTMENT_H
#define AMAZON_LOCKER_SERVICE_LLD_MODELS_LOCKERCOMPARTMENT_H

#include <string>

#include "../enums/CompartmentStatus.h"
#include "../enums/LockerSize.h"

namespace amazon_locker_lld {

class LockerCompartment {
public:
    LockerCompartment(std::string compartmentId, LockerSize size)
        : compartmentId_(std::move(compartmentId)),
          size_(size),
          status_(CompartmentStatus::AVAILABLE),
          packageId_("") {}

    const std::string& compartmentId() const { return compartmentId_; }
    LockerSize size() const { return size_; }
    CompartmentStatus status() const { return status_; }
    const std::string& packageId() const { return packageId_; }

    bool isAvailable() const { return status_ == CompartmentStatus::AVAILABLE; }

    void occupy(const std::string& packageId) {
        packageId_ = packageId;
        status_ = CompartmentStatus::OCCUPIED;
    }

    void release() {
        packageId_.clear();
        status_ = CompartmentStatus::AVAILABLE;
    }

    void markOutOfService() { status_ = CompartmentStatus::OUT_OF_SERVICE; }

private:
    std::string compartmentId_;
    LockerSize size_;
    CompartmentStatus status_;
    std::string packageId_;
};

}  // namespace amazon_locker_lld

#endif  // AMAZON_LOCKER_SERVICE_LLD_MODELS_LOCKERCOMPARTMENT_H

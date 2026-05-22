#ifndef AMAZON_LOCKER_SERVICE_LLD_STRATEGIES_FIRSTFITALLOCATIONSTRATEGY_H
#define AMAZON_LOCKER_SERVICE_LLD_STRATEGIES_FIRSTFITALLOCATIONSTRATEGY_H

#include <optional>
#include <string>

#include "../enums/CompartmentStatus.h"
#include "../enums/LockerSize.h"
#include "../models/LockerStation.h"
#include "ICompartmentAllocationStrategy.h"

namespace amazon_locker_lld {

inline bool fitsSize(LockerSize compartment, LockerSize required) {
    if (required == LockerSize::SMALL) {
        return true;
    }
    if (required == LockerSize::MEDIUM) {
        return compartment == LockerSize::MEDIUM || compartment == LockerSize::LARGE;
    }
    return compartment == LockerSize::LARGE;
}

class FirstFitAllocationStrategy : public ICompartmentAllocationStrategy {
public:
    std::optional<std::string> allocate(LockerStation& station, LockerSize requiredSize) override {
        for (auto& compartment : station.compartments()) {
            if (!compartment.isAvailable()) {
                continue;
            }
            if (!fitsSize(compartment.size(), requiredSize)) {
                continue;
            }
            return compartment.compartmentId();
        }
        return std::nullopt;
    }
};

}  // namespace amazon_locker_lld

#endif  // AMAZON_LOCKER_SERVICE_LLD_STRATEGIES_FIRSTFITALLOCATIONSTRATEGY_H

#ifndef AMAZON_LOCKER_SERVICE_LLD_STRATEGIES_ICOMPARTMENTALLOCATIONSTRATEGY_H
#define AMAZON_LOCKER_SERVICE_LLD_STRATEGIES_ICOMPARTMENTALLOCATIONSTRATEGY_H

#include <optional>
#include <string>

#include "../enums/LockerSize.h"
#include "../models/LockerStation.h"

namespace amazon_locker_lld {

class ICompartmentAllocationStrategy {
public:
    virtual ~ICompartmentAllocationStrategy() = default;

    virtual std::optional<std::string> allocate(LockerStation& station, LockerSize requiredSize) = 0;
};

}  // namespace amazon_locker_lld

#endif  // AMAZON_LOCKER_SERVICE_LLD_STRATEGIES_ICOMPARTMENTALLOCATIONSTRATEGY_H

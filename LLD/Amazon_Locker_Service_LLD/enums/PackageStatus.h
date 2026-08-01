#ifndef AMAZON_LOCKER_SERVICE_LLD_ENUMS_PACKAGESTATUS_H
#define AMAZON_LOCKER_SERVICE_LLD_ENUMS_PACKAGESTATUS_H

#include <string>

namespace amazon_locker_lld {

enum class PackageStatus { CREATED, DEPOSITED, PICKED_UP, EXPIRED };

inline std::string packageStatusToString(PackageStatus status) {
    switch (status) {
        case PackageStatus::CREATED:
            return "CREATED";
        case PackageStatus::DEPOSITED:
            return "DEPOSITED";
        case PackageStatus::PICKED_UP:
            return "PICKED_UP";
        case PackageStatus::EXPIRED:
            return "EXPIRED";
    }
    return "UNKNOWN";
}

}  // namespace amazon_locker_lld

#endif  // AMAZON_LOCKER_SERVICE_LLD_ENUMS_PACKAGESTATUS_H

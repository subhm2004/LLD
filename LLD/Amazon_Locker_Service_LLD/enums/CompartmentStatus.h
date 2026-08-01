#ifndef AMAZON_LOCKER_SERVICE_LLD_ENUMS_COMPARTMENTSTATUS_H
#define AMAZON_LOCKER_SERVICE_LLD_ENUMS_COMPARTMENTSTATUS_H

#include <string>

namespace amazon_locker_lld {

enum class CompartmentStatus { AVAILABLE, OCCUPIED, OUT_OF_SERVICE };

inline std::string compartmentStatusToString(CompartmentStatus status) {
    switch (status) {
        case CompartmentStatus::AVAILABLE:
            return "AVAILABLE";
        case CompartmentStatus::OCCUPIED:
            return "OCCUPIED";
        case CompartmentStatus::OUT_OF_SERVICE:
            return "OUT_OF_SERVICE";
    }
    return "UNKNOWN";
}

}  // namespace amazon_locker_lld

#endif  // AMAZON_LOCKER_SERVICE_LLD_ENUMS_COMPARTMENTSTATUS_H

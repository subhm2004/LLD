#ifndef AMAZON_LOCKER_SERVICE_LLD_ENUMS_ACCESSCODESTATUS_H
#define AMAZON_LOCKER_SERVICE_LLD_ENUMS_ACCESSCODESTATUS_H

#include <string>

namespace amazon_locker_lld {

enum class AccessCodeStatus { ACTIVE, USED, EXPIRED, LOCKED };

inline std::string accessCodeStatusToString(AccessCodeStatus status) {
    switch (status) {
        case AccessCodeStatus::ACTIVE:
            return "ACTIVE";
        case AccessCodeStatus::USED:
            return "USED";
        case AccessCodeStatus::EXPIRED:
            return "EXPIRED";
        case AccessCodeStatus::LOCKED:
            return "LOCKED";
    }
    return "UNKNOWN";
}

}  // namespace amazon_locker_lld

#endif  // AMAZON_LOCKER_SERVICE_LLD_ENUMS_ACCESSCODESTATUS_H

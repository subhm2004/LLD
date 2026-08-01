#ifndef AMAZON_LOCKER_SERVICE_LLD_ENUMS_LOCKERSIZE_H
#define AMAZON_LOCKER_SERVICE_LLD_ENUMS_LOCKERSIZE_H

#include <string>

namespace amazon_locker_lld {

enum class LockerSize { SMALL, MEDIUM, LARGE };

inline std::string lockerSizeToString(LockerSize size) {
    switch (size) {
        case LockerSize::SMALL:
            return "SMALL";
        case LockerSize::MEDIUM:
            return "MEDIUM";
        case LockerSize::LARGE:
            return "LARGE";
    }
    return "UNKNOWN";
}

}  // namespace amazon_locker_lld

#endif  // AMAZON_LOCKER_SERVICE_LLD_ENUMS_LOCKERSIZE_H

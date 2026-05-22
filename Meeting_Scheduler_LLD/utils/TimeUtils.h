#ifndef MEETING_SCHEDULER_LLD_UTILS_TIMEUTILS_H
#define MEETING_SCHEDULER_LLD_UTILS_TIMEUTILS_H

#include <stdexcept>
#include <string>

namespace meeting_scheduler_lld {

inline void validateDate(const std::string& date) {
    if (date.size() != 10 || date[4] != '-' || date[7] != '-') {
        throw std::invalid_argument("date must be YYYY-MM-DD: " + date);
    }
    for (size_t i = 0; i < date.size(); ++i) {
        if (i == 4 || i == 7) {
            continue;
        }
        if (date[i] < '0' || date[i] > '9') {
            throw std::invalid_argument("invalid date: " + date);
        }
    }
}

inline void validateMinutes(int minutes) {
    if (minutes < 0 || minutes > 1439) {
        throw std::invalid_argument("minutes must be in [0, 1439]");
    }
}

inline void validateTimeRange(int startMinutes, int endMinutes) {
    validateMinutes(startMinutes);
    validateMinutes(endMinutes);
    if (startMinutes >= endMinutes) {
        throw std::invalid_argument("start must be before end");
    }
}

inline bool rangesOverlap(int aStart, int aEnd, int bStart, int bEnd) {
    return aStart < bEnd && bStart < aEnd;
}

inline bool rangeContains(int outerStart, int outerEnd, int innerStart, int innerEnd) {
    return outerStart <= innerStart && innerEnd <= outerEnd;
}

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_UTILS_TIMEUTILS_H

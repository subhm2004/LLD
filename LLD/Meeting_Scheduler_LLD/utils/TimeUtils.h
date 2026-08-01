#ifndef MEETING_SCHEDULER_LLD_UTILS_TIMEUTILS_H
#define MEETING_SCHEDULER_LLD_UTILS_TIMEUTILS_H

#include <stdexcept>
#include <string>

namespace meeting_scheduler_lld {

/**
 * @file TimeUtils.h
 * @brief Date aur Minutes validations aur overlaps checking ke liye utility functions.
 * 
 * Hinglish comments:
 * - Isme time calculation aur formatting validation ke simple inline methods hain.
 */

// Date format (YYYY-MM-DD) validate karne ke liye helper method.
inline void validateDate(const std::string& date) {
    if (date.size() != 10 || date[4] != '-' || date[7] != '-') {
        throw std::invalid_argument("Date format YYYY-MM-DD hona chahiye: " + date);
    }
    for (size_t i = 0; i < date.size(); ++i) {
        if (i == 4 || i == 7) {
            continue;
        }
        if (date[i] < '0' || date[i] > '9') {
            throw std::invalid_argument("Invalid characters in date: " + date);
        }
    }
}

// Minutes (0-1439 range) validate karne ke liye.
inline void validateMinutes(int minutes) {
    if (minutes < 0 || minutes > 1439) {
        throw std::invalid_argument("Minutes 0 aur 1439 (midnight limits) ke range me hone chahiye.");
    }
}

// Start aur End minute range boundaries check karne ke liye.
inline void validateTimeRange(int startMinutes, int endMinutes) {
    validateMinutes(startMinutes);
    validateMinutes(endMinutes);
    if (startMinutes >= endMinutes) {
        throw std::invalid_argument("Start time humesha End time se pehle hona chahiye.");
    }
}

// Do time slots aapas me takra/overlap rahe hain ya nahi check karne ke liye.
inline bool rangesOverlap(int aStart, int aEnd, int bStart, int bEnd) {
    return aStart < bEnd && bStart < aEnd;
}

// Kya ek slot boundaries dusri slot boundaries ke complete andar fit hoti hai.
inline bool rangeContains(int outerStart, int outerEnd, int innerStart, int innerEnd) {
    return outerStart <= innerStart && innerEnd <= outerEnd;
}

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_UTILS_TIMEUTILS_H

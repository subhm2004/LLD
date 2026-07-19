#ifndef MEETING_SCHEDULER_LLD_ENUMS_MEETINGSTATUS_H
#define MEETING_SCHEDULER_LLD_ENUMS_MEETINGSTATUS_H

namespace meeting_scheduler_lld {

/**
 * @enum MeetingStatus
 * @brief Ek meeting apni lifetime me kaunse state me hai, ye represent karta hai.
 *
 * Jab meeting create/book hoti hai to iska status SCHEDULED hota hai. Agar organizer
 * ya koi participant use cancel kar deta hai to status CANCELLED ho jata hai.
 * Sirf SCHEDULED meetings hi conflict/overlap checks (isActive) me count hoti hain.
 */
enum class MeetingStatus {
    SCHEDULED,  // Meeting active hai — room booked hai aur slot block hai.
    CANCELLED   // Meeting cancel ho chuki hai — room/slot ab free maana jayega.
};

}  // namespace meeting_scheduler_lld

#endif  // MEETING_SCHEDULER_LLD_ENUMS_MEETINGSTATUS_H

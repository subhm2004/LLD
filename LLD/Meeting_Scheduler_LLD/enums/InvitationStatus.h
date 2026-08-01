#ifndef MEETING_SCHEDULER_LLD_ENUMS_INVITATIONSTATUS_H
#define MEETING_SCHEDULER_LLD_ENUMS_INVITATIONSTATUS_H

namespace meeting_scheduler_lld {

/**
 * @enum InvitationStatus
 * @brief Ek meeting invitation ki status state represent karta hai.
 * 
 * Jab kisi user ko meeting me invite kiya jata hai, toh uska status shuru me PENDING hota hai.
 * User chahe toh is invitation ko ACCEPT kar sakta hai ya DECLINE kar sakta hai.
 */
enum class InvitationStatus {
    PENDING,   // Abhi tak user ne status respond nahi kiya hai, decision waiting pe hai.
    ACCEPTED,  // User ne meeting invite ko accept kar liya hai aur calendar block kiya hai.
    DECLINED   // User ne meeting invite ko decline kar diya hai, matlab wo meeting me nahi aayega.
};

} // namespace meeting_scheduler_lld

#endif // MEETING_SCHEDULER_LLD_ENUMS_INVITATIONSTATUS_H

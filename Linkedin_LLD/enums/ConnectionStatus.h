// ============================================================================
//  enums/ConnectionStatus.h  —  Ek connection request ki zindagi ke stages
// ----------------------------------------------------------------------------
//  Requirement R3 (Connection Management): "Users can send, cancel, accept, or
//  ignore connection requests."
//
//  Ek request in me se kisi EK state me hoti hai:
//
//     PENDING   -> bhej di gayi, saamne wale ne abhi tak kuch decide nahi kiya
//     ACCEPTED  -> saamne wale ne haan kar di  -> dono ab CONNECTED hain
//     REJECTED  -> saamne wale ne saaf mana kiya (existing behavior, rakha hai)
//     CANCELLED -> BHEJNE WALE ne khud wapas le li (R3 ka "cancel")
//     IGNORED   -> paane wale ne chup-chaap side kar di (R3 ka "ignore")
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ REJECTED vs IGNORED vs CANCELLED — teeno alag kyun?                   │
//  │                                                                          │
//  │   REJECTED  : receiver ne active "NO" bola (formal reject)                │
//  │   IGNORED   : receiver ne dekha-andekha kar diya (LinkedIn ka "Ignore"    │
//  │               button) — request list se hat jaati hai, koi hard "no" nahi │
//  │   CANCELLED : SENDER ne apni bheji hui request wapas kheench li           │
//  │                                                                          │
//  │   Kaun action kar sakta hai — yahi teeno me farak hai:                    │
//  │      cancel  -> sirf FROM user (jisne bheji)                              │
//  │      accept/reject/ignore -> sirf TO user (jise mili)                     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 STATE MACHINE RULE: transition sirf PENDING se hota hai. Ek baar request
//     terminal state (ACCEPTED/REJECTED/CANCELLED/IGNORED) me chali gayi, wo
//     phir kabhi nahi badalti. Isse "already processed" wale duplicate actions
//     apne aap block ho jaate hain. (ConnectionRequest.h me enforce kiya hai.)
//
//  `enum class` (scoped) hai — `ConnectionStatus::PENDING` likhna padta hai,
//  naam bahar leak nahi hota aur `int` me chupke se convert nahi hota.
// ============================================================================
#ifndef LINKEDIN_LLD_ENUMS_CONNECTIONSTATUS_H
#define LINKEDIN_LLD_ENUMS_CONNECTIONSTATUS_H

#include <string>

namespace linkedin_lld {

enum class ConnectionStatus {
    PENDING,   // bheji gayi, faisla baaki
    ACCEPTED,  // haan -> ab dono connected
    REJECTED,  // saaf mana (existing)
    CANCELLED, // sender ne wapas le li (R3)
    IGNORED    // receiver ne andekha kiya (R3)
};

// Enum ko padhne-layak string me — printing/logging ke liye.
// `inline` isliye taaki ye header kai .cpp me include ho to linker "duplicate
// symbol" ka error na de.
inline std::string connectionStatusToString(ConnectionStatus status) {
    switch (status) {
        case ConnectionStatus::PENDING:   return "PENDING";
        case ConnectionStatus::ACCEPTED:  return "ACCEPTED";
        case ConnectionStatus::REJECTED:  return "REJECTED";
        case ConnectionStatus::CANCELLED: return "CANCELLED";
        case ConnectionStatus::IGNORED:   return "IGNORED";
    }
    return "UNKNOWN";
}

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_ENUMS_CONNECTIONSTATUS_H

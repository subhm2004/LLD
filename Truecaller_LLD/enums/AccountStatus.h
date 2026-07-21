// ============================================================================
//  enums/AccountStatus.h  —  User ka account chalu hai ya band
// ----------------------------------------------------------------------------
//     ACTIVE    -> sab normal
//     SUSPENDED -> account band (fake profile, mass-reporting abuse, etc.)
//
//  ⭐ Account DELETE ke bajaye SUSPEND kyun?
//     Kyunki user ke saath uska data juda hota hai — call logs, reports jo usne
//     kiye, directory entry. Delete karne se wo sab toot jaata (dangling
//     references) aur abuse ka audit trail bhi mit jaata. Suspend se banda rok
//     diya jaata hai par record bacha rehta hai.
//     📌 Ye "soft delete" wali soch hai — real systems me lagbhag hamesha yahi.
//
//  ⚠ HONEST NOTE: abhi ye enum aadha-adhura wired hai —
//     `TruecallerSystem::getUserOrThrow` (non-const wala) SUSPENDED pe throw
//     karta hai, par `validateUser` (jo lookup/search/block me use hota hai)
//     sirf "user exist karta hai kya" dekhta hai — status NAHI. Aur `setStatus`
//     ko koi bulata bhi nahi.
//     Yaani suspended user abhi bhi lookup/report kar sakta hai. Isse "fix"
//     karna ek POLICY decision hai (suspended banda read kar sake ya nahi?),
//     isliye behavior nahi badla — par jaanna zaroori hai. Interview me aisi
//     adhoori wiring khud pakadna bada plus point hota hai.
// ============================================================================
#ifndef TRUECALLER_LLD_ENUMS_ACCOUNTSTATUS_H
#define TRUECALLER_LLD_ENUMS_ACCOUNTSTATUS_H

namespace truecaller_lld {

enum class AccountStatus {
    ACTIVE,    // sab normal
    SUSPENDED  // ⚠ abhi sirf write-path pe check hota hai (upar note)
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_ENUMS_ACCOUNTSTATUS_H

// ============================================================================
//  enums/CallType.h  —  Call ki kism (call log ke liye)
// ----------------------------------------------------------------------------
//     INCOMING -> kisi ne kiya, uthaya
//     OUTGOING -> humne kiya
//     MISSED   -> aayi par uthi nahi
//
//  ⭐ MISSED alag value kyun (INCOMING ka flag kyun nahi)?
//     Kyunki user ke liye missed call ek ALAG cheez hai — usme action chahiye
//     hota hai (callback karna). Aur spam detection me MISSED sabse important
//     signal hai: spam callers aksar ek ring de ke kaat dete hain
//     ("one-ring scam" — taaki tum wapas call karo aur charge lage 💸).
//     Isliye ye analytics ka first-class input hai, sirf UI label nahi.
//
//  📌 `enum class` — type-safe. `CallType::MISSED` galti se int me nahi badlega.
// ============================================================================
#ifndef TRUECALLER_LLD_ENUMS_CALLTYPE_H
#define TRUECALLER_LLD_ENUMS_CALLTYPE_H

namespace truecaller_lld {

enum class CallType {
    INCOMING,  // aayi aur uthi
    OUTGOING,  // humne ki
    MISSED     // ⭐ aayi par uthi nahi (spam ka bada signal)
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_ENUMS_CALLTYPE_H

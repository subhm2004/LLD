// ============================================================================
//  models/CallerLookupResult.h  —  ⭐ Caller ID screen ka poora jawab
// ----------------------------------------------------------------------------
//  Jab phone bajta hai, screen pe jo kuch dikhta hai — wo sab is EK object me.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `struct` hai, `class` nahi — aur ye jaan-boojh ke hai                 │
//  │                                                                          │
//  │  Ye ek DTO (Data Transfer Object) hai: sirf data dhone ka bartan, koi     │
//  │  behavior nahi, koi invariant nahi. Aise objects ke liye getters/setters  │
//  │  likhna sirf shor hai — `struct` se seedha `result.spamScore` padho.     │
//  │                                                                          │
//  │  📌 Rule: INVARIANT ho (jaise "balance negative nahi ho sakta") to class  │
//  │     + private + validation. Sirf DATA CARRY karna ho to plain struct.    │
//  │     `PhoneProfile` class hai (uska score<->tag ka rule hai), ye struct.  │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ TEEN boolean flags — teeno ka matlab BILKUL alag hai                │
//  │                                                                          │
//  │     foundInDirectory -> "hum is number ke baare me kuch jaante hain?"     │
//  │     isSpam           -> "kya ye khatarnak hai?" (score>=70 ya SPAM tag)   │
//  │     isBlocked        -> "kya ISNE (is user ne) block kiya hai?"           │
//  │                                                                          │
//  │  ⭐ isSpam GLOBAL hai (poori duniya ka faisla), isBlocked PERSONAL hai    │
//  │     (sirf is user ka faisla). Isi liye lookup me `forUserId` chahiye —    │
//  │     ek hi number Alice ke liye blocked ho sakta hai aur Bob ke liye nahi. │
//  │                                                                          │
//  │  Aur `foundInDirectory=false` + `tag=UNKNOWN` -> bilkul anjaan number.    │
//  │  `foundInDirectory=true` + `tag=UNKNOWN` -> jaante hain (kisi ne report   │
//  │  kiya tha) par naam/label pata nahi. Sirf tag dekh ke ye farak nahi       │
//  │  pata chalta — isliye flag alag hai.                                     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Har field ka DEFAULT diya hua hai (`{}` initializers) — isse partially
//     bhara hua result kabhi garbage nahi dikhata. `LookupService` "number nahi
//     mila" wale case me seedha `return result;` kar deta hai aur sab kuch safe
//     defaults pe rehta hai (unknown, 0, false). Uninitialized read ka mauka nahi.
//
//  ⭐ Ek hi call me SAB kuch — caller ko lookup ke baad alag se "spam hai kya?",
//     "blocked hai kya?" poochne ki zaroorat nahi. Ek round-trip, poora jawab.
//     (Ye API design me "chatty vs chunky" ka classic trade-off hai.)
// ============================================================================
#ifndef TRUECALLER_LLD_MODELS_CALLERLOOKUPRESULT_H
#define TRUECALLER_LLD_MODELS_CALLERLOOKUPRESULT_H

#include <string>

#include "../enums/CallerTag.h"

namespace truecaller_lld {

struct CallerLookupResult {
    std::string phone;                    // normalized
    std::string displayName;              // "Unknown" agar pata nahi
    CallerTag tag{CallerTag::UNKNOWN};
    int spamScore{0};                     // 0..100
    bool isSpam{false};                   // ⭐ GLOBAL faisla (duniya ka)
    bool isBlocked{false};                // ⭐ PERSONAL faisla (is user ka)
    bool foundInDirectory{false};         // ⭐ "kuch jaante bhi hain?"
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_MODELS_CALLERLOOKUPRESULT_H

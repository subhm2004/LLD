// ============================================================================
//  services/LookupService.h  —  ⭐ CALLER ID (app ka sabse important feature)
// ----------------------------------------------------------------------------
//  Phone bajta hai -> ye method chalta hai -> screen pe naam/spam badge dikhta hai.
//  Poore Truecaller ka "wow moment" yahi hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ DO ALAG SOURCES ka jawab ek result me jud jaata hai                 │
//  │                                                                          │
//  │     directory (GLOBAL)   -> naam, tag, spam score — poori duniya ka data  │
//  │     blockService (LOCAL) -> "ISNE block kiya hai?" — sirf is user ka data │
//  │                                                                          │
//  │  Isi liye method me `forUserId` chahiye. Ek hi number:                    │
//  │     Alice ke liye -> blocked=yes                                          │
//  │     Bob ke liye   -> blocked=no                                           │
//  │  ...par spam score DONO ke liye SAME (wo global faisla hai).             │
//  │  📌 Ye "global truth + personal preference" wala pattern har social/      │
//  │     communication app me milta hai (mute, block, hide — sab personal).   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `isBlocked` directory lookup se PEHLE set hota hai — soch samajh ke   │
//  │                                                                          │
//  │  Number directory me ho ya na ho, BLOCK to phir bhi lagna chahiye.        │
//  │  Agar block check `directory.find()` ke andar hota, to anjaan number      │
//  │  block karne ke baad bhi call aati rehti! 😱                              │
//  │  Isliye pehle block check, phir early-return wala directory lookup.       │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Do "Unknown" fallbacks hain — dono zaroori:
//     1. number directory me hai hi nahi -> "Unknown" + early return
//     2. profile hai par `displayName` khaali (sirf report hua tha, naam pata
//        nahi) -> bhi "Unknown", PAR `foundInDirectory=true` aur spam score
//        bhara hua. Yaani "naam nahi pata, magar ye number spam hai" — jo
//        Truecaller ka sabse kaam ka jawab hai!
//
//  ⭐ Poora method `const` hai aur saara data PARAMETERS se aata hai — service
//     ka apna koi state nahi. Isse: test karna trivial (fake directory pass
//     karo), thread-safety ki tension nahi, aur ek hi object sab share kar sakte.
//     📌 Stateless services default choice honi chahiye.
//
//  ⚠ `normalizePhone(callerPhone)` yahan bhi hota hai — kyunki caller kahin se
//     bhi kachcha number bhej sakta hai. Directory ka key normalized hai, to
//     query bhi normalized honi chahiye, warna match kabhi nahi hoga.
// ============================================================================
#ifndef TRUECALLER_LLD_SERVICES_LOOKUPSERVICE_H
#define TRUECALLER_LLD_SERVICES_LOOKUPSERVICE_H

#include <string>
#include <unordered_map>

#include "../models/CallerLookupResult.h"
#include "../models/PhoneProfile.h"
#include "../utils/PhoneUtils.h"
#include "BlockService.h"

namespace truecaller_lld {

class LookupService {
public:
    CallerLookupResult lookup(const std::string& forUserId, const std::string& callerPhone,
                              const std::unordered_map<std::string, PhoneProfile>& directory,
                              const BlockService& blockService) const {
        const std::string phone = normalizePhone(callerPhone); // ⭐ key ka format match karo
        CallerLookupResult result;
        result.phone = phone;

        // ⭐ BLOCK CHECK PEHLE — anjaan number bhi blocked ho sakta hai (upar note).
        result.isBlocked = blockService.isBlocked(forUserId, phone);

        auto it = directory.find(phone); // O(1) — yahi "low latency lookup" hai
        if (it == directory.end()) {
            // Bilkul anjaan number. Baaki fields apne safe defaults pe hain.
            result.displayName = "Unknown";
            result.tag = CallerTag::UNKNOWN;
            return result;
        }

        const PhoneProfile& profile = it->second;
        result.foundInDirectory = true; // ⭐ "kuch to jaante hain"
        // Profile hai par naam nahi (sirf report hua tha) -> naam Unknown,
        // magar spam score/tag bharpoor kaam ka hai.
        result.displayName = profile.getDisplayName().empty() ? "Unknown" : profile.getDisplayName();
        result.tag = profile.getPrimaryTag();
        result.spamScore = profile.getSpamScore();
        result.isSpam = profile.isSpam(); // ⭐ GLOBAL faisla
        return result;
    }
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_SERVICES_LOOKUPSERVICE_H

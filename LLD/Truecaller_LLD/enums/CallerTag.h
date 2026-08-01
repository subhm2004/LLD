// ============================================================================
//  enums/CallerTag.h  —  Number ke saath dikhne wala LABEL (caller ID badge)
// ----------------------------------------------------------------------------
//     PERSONAL     -> aam aadmi ka registered number
//     BUSINESS     -> dukaan/company ka number (registration me isBusiness=true)
//     SPAM         -> itne reports aa gaye ki score threshold (70) paar kar gaya
//     TELEMARKETER -> "sales call" — spam se HALKA label
//     UNKNOWN      -> directory me hai par label pata nahi
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ SPAM aur TELEMARKETER alag kyun hain?                                 │
//  │                                                                          │
//  │  Kyunki user ke liye dono ka matlab alag hai:                            │
//  │     TELEMARKETER -> "sales wala hai, chaho to mat uthao" (annoying)       │
//  │     SPAM         -> "yakeenan fraud/scam hai, MAT uthao" (khatarnak)      │
//  │                                                                          │
//  │  SpamReportService me logic ye hai: TELEMARKETER report aayi par score    │
//  │  abhi 70 se KAM hai -> tag TELEMARKETER. Score 70 paar -> tag SPAM        │
//  │  (`PhoneProfile::addSpamReport` khud upgrade kar deta hai).               │
//  │  📌 Yaani ek hi number pehle TELEMARKETER dikhta hai, aur reports         │
//  │     badhne pe apne aap SPAM me promote ho jaata hai.                     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ UNKNOWN ka DO matlab hota hai (context se pata chalta hai):
//     - directory me hai hi nahi (lookup me `foundInDirectory=false` aayega)
//     - directory me hai par label decide nahi hua (naya reported number)
//     Isi liye `CallerLookupResult` me `foundInDirectory` alag flag hai —
//     sirf tag dekh ke ye farak nahi pata chalta.
// ============================================================================
#ifndef TRUECALLER_LLD_ENUMS_CALLERTAG_H
#define TRUECALLER_LLD_ENUMS_CALLERTAG_H

namespace truecaller_lld {

enum class CallerTag {
    PERSONAL,      // aam registered user
    BUSINESS,      // dukaan/company
    SPAM,          // ⭐ score >= 70 (khatarnak)
    TELEMARKETER,  // ⭐ sales call (annoying, par spam se halka)
    UNKNOWN        // pata nahi / directory me nahi
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_ENUMS_CALLERTAG_H

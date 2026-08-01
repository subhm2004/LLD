// ============================================================================
//  enums/ReportReason.h  —  User ne number ko KYUN report kiya
// ----------------------------------------------------------------------------
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Reason sirf label nahi — ye SPAM SCORE ka INPUT hai                   │
//  │                                                                          │
//  │  `DefaultSpamScoringStrategy` har reason ka alag WEIGHT deta hai:        │
//  │      SCAM         -> +35   (sabse khatarnak — paisa loot lete hain)      │
//  │      HARASSMENT   -> +30   (serious)                                     │
//  │      TELEMARKETER -> +25   (annoying)                                    │
//  │      ROBOT_CALL   -> +20   (automated)                                   │
//  │      OTHER        -> +15   (pata nahi kya — sabse kam bharosa)           │
//  │                                                                          │
//  │  Isliye 2 SCAM reports (35+35=70) turant SPAM tag laga dete hain, jabki   │
//  │  OTHER se 5 reports (15x5=75) lagti hain. Ye sahi hai — "scam" wali      │
//  │  report zyada specific aur zyada bharosemand hoti hai.                   │
//  │                                                                          │
//  │  📌 Aur ye weights kahin HARDCODED nahi hain — Strategy ke andar hain.    │
//  │     Kal ko ML model se score aaye to sirf nayi strategy class banegi.    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `OTHER` ka weight sabse KAM hai — jaan-boojh ke. Jab user specific reason
//     nahi batata to hume kam bharosa hota hai; usse ek number ko turant SPAM
//     mark karna galat hoga (koi apne dushman ka number report kar sakta hai).
//     📌 "Kam information = kam impact" — ye abuse se bachne ka simple rule hai.
// ============================================================================
#ifndef TRUECALLER_LLD_ENUMS_REPORTREASON_H
#define TRUECALLER_LLD_ENUMS_REPORTREASON_H

namespace truecaller_lld {

enum class ReportReason {
    TELEMARKETER,  // +25 — sales call
    SCAM,          // +35 — sabse bhaari (fraud)
    ROBOT_CALL,    // +20 — automated/IVR
    HARASSMENT,    // +30 — serious
    OTHER          // +15 — ⭐ sabse halka (kam bharosa)
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_ENUMS_REPORTREASON_H

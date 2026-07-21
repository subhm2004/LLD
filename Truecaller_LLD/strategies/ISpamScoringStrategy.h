// ============================================================================
//  strategies/ISpamScoringStrategy.h  —  ⭐ STRATEGY interface (spam ka dimaag)
// ----------------------------------------------------------------------------
//  Ek hi sawaal ka jawab: "is reason ki report se score kitna badhna chahiye?"
//
//      virtual int scoreDeltaFor(ReportReason reason) const = 0;
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Ye interface itna PATLA kyun hai (sirf 1 method)?                     │
//  │                                                                          │
//  │  Kyunki system ka sabse ZYADA BADALNE WALA hissa yahi hai. Spam detection │
//  │  hamesha evolve karta rehta hai:                                          │
//  │      aaj  -> fixed weights (SCAM=35, TELEMARKETER=25...)                  │
//  │      kal  -> reporter ki reputation bhi dekho (naya user vs purana)       │
//  │      parso-> call pattern (100 calls/min = robocall), time-decay          │
//  │      aage -> poora ML model                                              │
//  │                                                                          │
//  │  In sab me `SpamReportService` ka code EK LINE bhi nahi badlega — wo bas  │
//  │  `strategy_->scoreDeltaFor(reason)` bulata hai. Nayi class banao, inject  │
//  │  kar do, ho gaya. 📌 Yahi Open/Closed Principle hai.                     │
//  │                                                                          │
//  │  ⭐ Interface jitna PATLA, replace karna utna AASAN. Agar isme 10 methods │
//  │     hote to har nayi strategy me 10 likhne padte — log Strategy use hi    │
//  │     nahi karte.                                                          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⚠ HONEST LIMITATION: ye interface sirf `ReportReason` leta hai. Yaani aisi
//     strategy nahi ban sakti jo "kisne report kiya" ya "abhi ka score kya hai"
//     dekhe. Reputation-based scoring ke liye signature badalna padega:
//        `scoreDeltaFor(reason, reporterId, currentProfile)`
//     📌 Ye interface design ka asli sabak hai: interface tay karta hai ki
//        AAGE kya-kya possible hai. Bahut patla rakho to lachak kho jaati hai,
//        bahut mota rakho to implement karna mushkil. Interview me ye trade-off
//        khud bolna bada plus point hai.
//
//  ⭐ `virtual ~ISpamScoringStrategy() = default;` — MISS mat karna. Object
//     `shared_ptr<ISpamScoringStrategy>` se delete hota hai; destructor virtual
//     na ho to derived ka destructor chalta hi nahi (leak + UB).
//
//  📌 `const` method — scoring ek PURE calculation hai, strategy ka apna koi
//     state nahi badalta. Isliye ek hi strategy object saare users ke liye
//     safely share ho sakta hai (`shared_ptr` isi liye chalta hai).
// ============================================================================
#ifndef TRUECALLER_LLD_STRATEGIES_ISPAMSCORINGSTRATEGY_H
#define TRUECALLER_LLD_STRATEGIES_ISPAMSCORINGSTRATEGY_H

#include "../enums/ReportReason.h"

namespace truecaller_lld {

class ISpamScoringStrategy {
public:
    virtual ~ISpamScoringStrategy() = default; // ⭐ virtual (upar note)

    // "Is reason pe score kitna badhe?" — bas yahi ek sawaal.
    virtual int scoreDeltaFor(ReportReason reason) const = 0;
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_STRATEGIES_ISPAMSCORINGSTRATEGY_H

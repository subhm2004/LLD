// ============================================================================
//  strategies/DefaultSpamScoringStrategy.h  —  Simple rule-based scoring
// ----------------------------------------------------------------------------
//  Har reason ka ek FIXED weight. Bas ek lookup table hai — aur yahi is design
//  ki khoobi hai: sabse simple implementation, par pluggable.
//
//      SCAM         +35   ┐  do reports (35+35=70) = TURANT spam tag
//      HARASSMENT   +30   │  teen reports = 90
//      TELEMARKETER +25   │  teen reports = 75
//      ROBOT_CALL   +20   │  chaar reports = 80
//      OTHER        +15   ┘  paanch reports = 75
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Ye numbers RANDOM nahi hain — inke peeche soch hai                    │
//  │                                                                          │
//  │  Threshold 70 hai. Weights aise chune gaye hain ki:                      │
//  │   • SCAM ke liye SIRF 2 reports kaafi hain — kyunki fraud sabse           │
//  │     khatarnak hai aur ek call me hi paisa jaa sakta hai. Yahan            │
//  │     "der se sahi hona" ka koi fayda nahi.                                │
//  │   • OTHER ke liye 5 reports chahiye — kyunki reason specific nahi hai,    │
//  │     bharosa kam hai. Koi apne dushman ka number report kar sakta hai. 😈  │
//  │                                                                          │
//  │  📌 Yaani weight = "is signal pe kitna bharosa hai" ka paimaana.          │
//  │     Zyada specific report = zyada bharosa = zyada weight.                │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `switch` me `default:` NAHI hai — jaan-boojh ke!
//     `ReportReason` me nayi value add karo aur yahan case bhoolo, to compiler
//     `-Wswitch` warning dega. `default:` likh dete to compiler chup rehta aur
//     naya reason chup-chaap 10 (fallback) score paata — bug runtime pe milta. 😱
//     (Neeche wala `return 10;` sirf "control reaches end" wali warning ke liye
//      hai — corrupted enum value ka safe fallback.)
//
//  ⚠ Is simple strategy ki asli kamzoriyan (interview me khud bolna):
//     • REPEAT REPORTS: ek hi banda 3 baar report kare to score 105 -> ek user
//       akela kisi ka number SPAM mark kar sakta hai. Fix: (reporter, phone)
//       pair ko dedupe karo — SpamReportService me bhi note hai.
//     • NO TIME DECAY: 2 saal purani report aaj bhi utni hi bhaari hai. Asli
//       system me purane reports ka weight dheere-dheere kam hota hai.
//     • NO REPUTATION: naye account aur 5 saal purane trusted user ki report
//       ka weight same hai.
//     Yahi wajah hai ki ye Strategy ke peeche hai — inme se koi bhi improvement
//     ek NAYI class hai, is file ka edit nahi.
// ============================================================================
#ifndef TRUECALLER_LLD_STRATEGIES_DEFAULTSPAMSCORINGSTRATEGY_H
#define TRUECALLER_LLD_STRATEGIES_DEFAULTSPAMSCORINGSTRATEGY_H

#include "ISpamScoringStrategy.h"

namespace truecaller_lld {

class DefaultSpamScoringStrategy : public ISpamScoringStrategy {
public:
    int scoreDeltaFor(ReportReason reason) const override {
        switch (reason) { // ⭐ koi `default:` nahi (upar note)
            case ReportReason::SCAM:
                return 35; // ⭐ 2 reports = 70 = turant SPAM
            case ReportReason::TELEMARKETER:
                return 25;
            case ReportReason::ROBOT_CALL:
                return 20;
            case ReportReason::HARASSMENT:
                return 30;
            case ReportReason::OTHER:
                return 15; // ⭐ sabse halka (kam bharosa)
        }
        return 10; // corrupted enum value ka safe fallback
    }
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_STRATEGIES_DEFAULTSPAMSCORINGSTRATEGY_H

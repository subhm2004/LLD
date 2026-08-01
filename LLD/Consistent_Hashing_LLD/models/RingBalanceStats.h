// ============================================================================
//  models/RingBalanceStats.h  —  Ring kitna BALANCED hai, uska score
// ----------------------------------------------------------------------------
//  "Distribution achha lag raha hai" — ye kaafi nahi hai. Tuning ke liye ek
//  NUMBER chahiye jise aap minimize kar sako. Ye value object wahi deta hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ STANDARD DEVIATION — yahi wo number hai jise MINIMIZE karna hai       │
//  │                                                                          │
//  │  Har node ka "expected share" hota hai (uske weight ke hisaab se). Actual │
//  │  share usse thoda upar-neeche hoga. Std-dev batata hai ki AUSATAN kitna   │
//  │  bhatak rahe hain:                                                       │
//  │                                                                          │
//  │      stdDev = sqrt( mean( (actual - expected)² ) )                       │
//  │                                                                          │
//  │      stdDev = 0    -> perfect (har node ko theek utna mila jitna banta)   │
//  │      stdDev badhta -> ring utni hi tedhi                                  │
//  │                                                                          │
//  │  Virtual nodes badhane se ye number GIRTA hai — aur yahi vnodes ka poora  │
//  │  maqsad hai. Demo me aap ye girta hua dekh sakte ho.                     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ WEIGHTS KA DHYAN RAKHA GAYA HAI (ye zaroori hai):
//     Agar sab nodes ka weight barabar ho to expected share = 100/N. Par ek node
//     ka weight 3 ho to usko 3 guna milna CHAHIYE — wo "imbalance" nahi hai.
//     Isliye deviation `actual - expected` se naapte hain, `actual - 100/N` se
//     nahi. Warna weighted cluster hamesha "kharaab" dikhta, jo galat hai.
//
//  ⭐ `worstRatio` = sabse bura node apne expected se kitna guna hai.
//        1.00 = perfect  |  1.15 = koi node 15% zyada bhara hai
//     Std-dev "ausat" batata hai, ye "sabse bura case" batata hai. Dono chahiye —
//     ausat achha ho par ek node 2x bhara ho, to bhi problem hai.
// ============================================================================
#ifndef CONSISTENT_HASHING_LLD_MODELS_RINGBALANCESTATS_H
#define CONSISTENT_HASHING_LLD_MODELS_RINGBALANCESTATS_H

#include <cstddef>

namespace consistent_hashing_lld {

struct RingBalanceStats {
    size_t nodeCount = 0;
    double minSharePercent = 0.0; // sabse khaali node ka hissa
    double maxSharePercent = 0.0; // sabse bhare node ka hissa
    double stdDevPercent = 0.0;   // ⭐ ISE MINIMIZE KARNA HAI (0 = perfect)
    double worstRatio = 0.0;      // sabse bura node / uska expected (1.0 = perfect)
};

} // namespace consistent_hashing_lld

#endif // CONSISTENT_HASHING_LLD_MODELS_RINGBALANCESTATS_H

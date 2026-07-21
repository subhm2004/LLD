// ============================================================================
//  services/SpamReportService.h  —  Spam report lena aur score badhana
// ----------------------------------------------------------------------------
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ SABSE IMPORTANT LINE: report se PROFILE BAN JAATA HAI               │
//  │                                                                          │
//  │      if (it == directory.end()) {                                         │
//  │          directory.emplace(normalized,                                    │
//  │              PhoneProfile(normalized, "Reported Number", "", UNKNOWN));   │
//  │      }                                                                    │
//  │                                                                          │
//  │  Yaani directory me entry banane ke liye kisi ka REGISTER hona ZAROORI    │
//  │  NAHI. Ek anjaan spam number pe pehli report aate hi uska profile ban     │
//  │  jaata hai (`ownerUserId` khaali).                                       │
//  │                                                                          │
//  │  ⭐ Aur yahi Truecaller ka POORA BUSINESS MODEL hai: directory users ke   │
//  │     REPORTS se banti hai, registrations se nahi. Sirf registered users    │
//  │     wali directory bekaar hoti — spammer to kabhi register hi nahi karega!│
//  │  📌 Isi ko "crowd-sourced data" kehte hain — jitne zyada users, utni      │
//  │     achhi directory. Network effect.                                     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Kaam ka batwara — teen jagah, teen zimmedari                          │
//  │                                                                          │
//  │     Strategy      -> "kitna score badhe?"  (SCAM=35, OTHER=15...)         │
//  │     PhoneProfile  -> "score kaise apply ho?" (clamp 100, tag upgrade)     │
//  │     ye Service    -> "profile hai? nahi to banao" + special tag rules     │
//  │                                                                          │
//  │  Teeno alag isliye ki teeno alag-alag wajah se badalte hain.             │
//  │  📌 SRP ka asli matlab yahi hai: "ek class, badalne ki ek wajah".        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ TELEMARKETER wala special rule:
//        if (reason == TELEMARKETER && score < 70) setPrimaryTag(TELEMARKETER)
//     Yaani jab tak score threshold se neeche hai, number "SPAM" nahi balki
//     "TELEMARKETER" dikhta hai — halka label. Score 70 paar hote hi
//     `addSpamReport` khud SPAM me upgrade kar chuka hota hai, aur ye `if`
//     tab chalta hi nahi (isliye SPAM tag ko wapas TELEMARKETER nahi karta).
//     ⭐ Condition ka order dhyan se dekho — `< 70` na hota to ye line ek
//        upgraded SPAM tag ko DOWNGRADE kar deti. Chhoti si guard, bada kaam.
//
//  ⚠⚠ ASLI GAP (jaan-boojh ke chhoda, kyunki fix karna feature hai bug nahi):
//     **ek hi user BAAR-BAAR report kar sakta hai.** `reporterUserId` sirf
//     validate hota hai, kahin STORE nahi hota. Matlab ek banda 2 baar SCAM
//     report kar ke kisi ka bhi number SPAM mark kar sakta hai. 😈
//     Fix: `unordered_set<pair<reporter, phone>>` rakho aur duplicate report
//     ignore/skip karo. Real Truecaller me ye dedupe + reporter reputation +
//     time-decay sab hota hai. Interview me ye khud bolna bada plus point hai.
//
//  ⭐ `shared_ptr<ISpamScoringStrategy>` INJECTED hai (service khud nahi banati)
//     — isi wajah se test me fake strategy daal ke exact score verify kiya ja
//     sakta hai, aur production me strategy runtime pe swap ho sakti hai.
// ============================================================================
#ifndef TRUECALLER_LLD_SERVICES_SPAMREPORTSERVICE_H
#define TRUECALLER_LLD_SERVICES_SPAMREPORTSERVICE_H

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include "../enums/CallerTag.h"
#include "../enums/ReportReason.h"
#include "../models/PhoneProfile.h"
#include "../strategies/ISpamScoringStrategy.h"
#include "../utils/PhoneUtils.h"

namespace truecaller_lld {

class SpamReportService {
public:
    // Strategy injected — scoring ka dimaag bahar se aata hai (swappable).
    explicit SpamReportService(std::shared_ptr<ISpamScoringStrategy> strategy)
        : strategy_(std::move(strategy)) {}

    void reportSpam(const std::string& reporterUserId, const std::string& phone,
                    ReportReason reason,
                    std::unordered_map<std::string, PhoneProfile>& directory) {
        if (reporterUserId.empty()) {
            throw std::invalid_argument("reporter user id required");
        }
        // ⚠ reporterUserId sirf validate hota hai, store NAHI — repeat-report
        //    ka gap yahi hai (upar poora note).
        const std::string normalized = normalizePhone(phone);

        // ⭐⭐ Profile nahi hai? Bana do — registration ki zaroorat NAHI.
        // Crowd-sourced directory ka dil yahi hai (upar note).
        auto it = directory.find(normalized);
        if (it == directory.end()) {
            directory.emplace(normalized,
                              PhoneProfile(normalized, "Reported Number", "", CallerTag::UNKNOWN));
            it = directory.find(normalized);
        }

        // Strategy se weight lo, PhoneProfile khud apply karega (clamp + upgrade).
        const int delta = strategy_->scoreDeltaFor(reason);
        it->second.addSpamReport(delta);

        // ⭐ Threshold se NEECHE hai to halka label (TELEMARKETER). `< 70` guard
        // isliye ki upgraded SPAM tag kabhi downgrade na ho (upar note).
        if (reason == ReportReason::TELEMARKETER && it->second.getSpamScore() < 70) {
            it->second.setPrimaryTag(CallerTag::TELEMARKETER);
        }
    }

private:
    std::shared_ptr<ISpamScoringStrategy> strategy_; // ⭐ swappable dimaag
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_SERVICES_SPAMREPORTSERVICE_H

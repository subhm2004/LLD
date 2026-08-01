// ============================================================================
//  models/PhoneProfile.h  —  ⭐ GLOBAL DIRECTORY ka ek entry (system ka core)
// ----------------------------------------------------------------------------
//  Ek phone number ke baare me duniya kya jaanti hai: naam, label, spam score.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ PhoneProfile vs User — ye farak samajhna SABSE ZAROORI hai          │
//  │                                                                          │
//  │     User         -> jo banda APP me registered hai (account, email)      │
//  │     PhoneProfile -> KISI BHI number ki public jaankari                   │
//  │                                                                          │
//  │  Har PhoneProfile ka User HONA ZAROORI NAHI! Jo number kabhi register     │
//  │  hi nahi hua par logon ne spam report kiya — uska bhi PhoneProfile banta  │
//  │  hai (SpamReportService khud bana deta hai, `ownerUserId_` khaali).      │
//  │                                                                          │
//  │  Yahi to Truecaller ka poora business hai — WO number pehchanna jo aapki  │
//  │  contact list me nahi hai aur jisne kabhi app install nahi kiya.          │
//  │  📌 Isliye directory ka key `phone` hai, `userId` nahi.                  │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ `isSpam()` me OR hai — do alag raaste                                │
//  │      spamScore_ >= 70   ->  bahut saare logon ne report kiya (crowd)     │
//  │      primaryTag_ == SPAM ->  manually/admin ne SPAM mark kiya            │
//  │  Dono independent hain: koi bhi ek sach ho to number spam hai.           │
//  │  Threshold 70 hai — 100 nahi, kyunki 100% surety ka wait karoge to spam  │
//  │  call uth chuki hogi. 70 = "kaafi log bol chuke hain, warn kar do".      │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `addSpamReport` me `std::min(100, ...)` — score kabhi 100 se upar nahi
//     jaata. Bina clamp ke ek number ka score 5000 ho jaata; phir usko "clean"
//     karna (galat report hui thi) practically namumkin ho jaata. Bounded score
//     ka matlab hai bounded recovery.
//
//  ⭐ `addSpamReport` khud tag UPGRADE karta hai (score 70 paar -> SPAM). Ye
//     rule model ke andar hai, service me nahi — kyunki score aur tag ka rishta
//     is class ka INVARIANT hai. Service me rakhte to koi caller bhool sakta tha.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  🐛 BUG FIX: `claimOwnership()` — pehle register karne se spam history    │
//  │              MIT jaati thi (spam evasion ka raasta)                       │
//  │                                                                          │
//  │  PEHLE `TruecallerSystem::registerUser` seedha ye karta tha:              │
//  │      directory_[phone] = PhoneProfile(phone, name, userId, tag, 0);       │
//  │  Yaani PURA profile OVERWRITE — spamScore aur reportCount dono ZERO. 😱   │
//  │                                                                          │
//  │  Attack: spammer call karta rahe, log report karte rahein (score 70,      │
//  │  tag SPAM) -> phir spammer khud app install karke us number se REGISTER   │
//  │  kar le -> saare reports MIT gaye, number saaf. Dobara wahi khel.        │
//  │                                                                          │
//  │  CONFIRM kiya tha: score 70 -> register -> score 0. ✅ bug                │
//  │                                                                          │
//  │  FIX: ye method identity (naam/owner/tag) update karta hai par            │
//  │  `spamScore_` aur `reportCount_` ko HAATH NAHI LAGATA. Aur agar number    │
//  │  pehle se spam hai to tag bhi SPAM hi rehta hai — profile edit karke      │
//  │  badge nahi hataya ja sakta.                                             │
//  │  📌 RULE: user khud ka DATA badal sakta hai, par DOOSRON KA FAISLA nahi.  │
//  │     Reports doosre logon ne diye hain — wo unki property hain.           │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef TRUECALLER_LLD_MODELS_PHONEPROFILE_H
#define TRUECALLER_LLD_MODELS_PHONEPROFILE_H

#include <algorithm>  // std::min (pehle PhoneUtils se transitively aata tha — fragile)
#include <string>
#include <utility>

#include "../enums/CallerTag.h"
#include "../utils/PhoneUtils.h"

namespace truecaller_lld {

/** Global directory entry for a phone number (registered ho ya na ho). */
class PhoneProfile {
public:
    PhoneProfile() = default;

    // Sirf number pata hai — koi naam/label nahi (bare-bones entry).
    explicit PhoneProfile(std::string phone) : phone_(normalizePhone(std::move(phone))) {}

    PhoneProfile(std::string phone, std::string displayName, std::string ownerUserId, CallerTag tag,
                 int spamScore = 0)
        : phone_(normalizePhone(std::move(phone))), // ⭐ hamesha canonical form
          displayName_(std::move(displayName)),
          ownerUserId_(std::move(ownerUserId)),     // khaali = koi registered maalik nahi
          primaryTag_(tag),
          spamScore_(spamScore) {}

    const std::string& getPhone() const { return phone_; }
    const std::string& getDisplayName() const { return displayName_; }
    const std::string& getOwnerUserId() const { return ownerUserId_; }
    CallerTag getPrimaryTag() const { return primaryTag_; }
    int getSpamScore() const { return spamScore_; }
    int getReportCount() const { return reportCount_; }

    // ⭐ Do raaste — crowd score YA explicit tag (upar note).
    bool isSpam() const { return spamScore_ >= 70 || primaryTag_ == CallerTag::SPAM; }

    void setDisplayName(const std::string& name) { displayName_ = name; }
    void setPrimaryTag(CallerTag tag) { primaryTag_ = tag; }

    // ⭐ Report aaya: count badhao, score badhao (clamped), aur zaroorat pade
    //    to tag ko SPAM me UPGRADE karo. Score<->tag ka rishta yahin sambhala
    //    jaata hai (invariant model ke andar).
    void addSpamReport(int scoreDelta) {
        reportCount_++;
        spamScore_ = std::min(100, spamScore_ + scoreDelta); // ⭐ 100 pe cap
        if (spamScore_ >= 70) {
            primaryTag_ = CallerTag::SPAM;
        }
    }

    // ⭐⭐ BUG FIX: koi is number ka MAALIK ban raha hai (register/profile update).
    // Identity badalti hai — par SPAM HISTORY nahi mitti (upar poora note).
    void claimOwnership(const std::string& ownerUserId, const std::string& displayName,
                        CallerTag tag) {
        ownerUserId_ = ownerUserId;
        displayName_ = displayName;
        // ⭐ Number pehle se spam hai? To badge hataya nahi ja sakta.
        // Profile edit karke spam label saaf karne ka raasta band.
        if (!isSpam()) {
            primaryTag_ = tag;
        }
        // spamScore_ / reportCount_ ko haath BHI nahi lagate. 🔒
    }

private:
    std::string phone_;        // ⭐ directory ka key (canonical form)
    std::string displayName_;
    std::string ownerUserId_;  // khaali = unregistered number (bilkul valid case)
    CallerTag primaryTag_{CallerTag::UNKNOWN};
    int spamScore_{0};         // 0..100 (70 = threshold)
    int reportCount_{0};       // kitne logon ne report kiya
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_MODELS_PHONEPROFILE_H

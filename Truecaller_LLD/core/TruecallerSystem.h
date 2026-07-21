// ============================================================================
//  core/TruecallerSystem.h  —  FACADE (poore system ka ek darwaza)
// ----------------------------------------------------------------------------
//  Andar 6 services + 1 strategy hain. `main.cpp` ko ek ka bhi naam nahi pata —
//  bas `system.identifyCaller(...)`, `system.reportSpam(...)`.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Facade ka kaam (aur jo NAHI hai)                                      │
//  │                                                                          │
//  │   KARTA HAI:                                                             │
//  │     - saara DATA own karta hai (users, directory, contacts, lookups)     │
//  │     - services ko own + wire karta hai                                    │
//  │     - cross-cutting validation (user exist karta hai? naam khaali to nahi?)│
//  │     - DO JAGAH ka data sync me rakhna (User <-> PhoneProfile)            │
//  │     - id counters                                                        │
//  │                                                                          │
//  │   NAHI KARTA:                                                            │
//  │     - spam scoring (Strategy), lookup logic (LookupService),             │
//  │       search (SearchService), block (BlockService)                       │
//  │  📌 Facade "coordinator" hai, "God object" nahi.                        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐⭐ DO SOURCE OF TRUTH — `users_` aur `directory_` (aur unka rishta)    │
//  │                                                                          │
//  │     users_[userId]     -> account data (email, status, isBusiness)       │
//  │     directory_[phone]  -> public data (naam, tag, spam score)            │
//  │                                                                          │
//  │  Naam DONO jagah hai — ye DUPLICATION hai, aur soch-samajh ke hai:       │
//  │   ✅ Fayda: caller ID lookup EK map se O(1) me ho jaata hai. Agar naam    │
//  │      sirf `users_` me hota to har call pe phone->userId->user ka do-hop   │
//  │      join karna padta. Aur unregistered numbers (jo sirf report hue hain) │
//  │      ka to `users_` me entry hoti hi nahi!                               │
//  │   ⚠ Keemat: `updateProfile` me DONO ko saath update karna PADTA hai,      │
//  │      warna directory me purana naam pada rehta.                          │
//  │  📌 Ye classic "denormalization for read performance" hai — read-heavy    │
//  │     system (har call pe lookup!) me bilkul sahi trade-off.               │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  🐛 BUG FIX: register/update se SPAM HISTORY mit jaati thi                │
//  │                                                                          │
//  │  PEHLE `registerUser` me ye tha:                                          │
//  │      directory_[normalized] = PhoneProfile(normalized, name, userId,      │
//  │                                            tag, 0);   // ⚠ pura overwrite │
//  │                                                                          │
//  │  Attack: number pe reports jama ho (score 70, tag SPAM) -> spammer khud   │
//  │  us number se REGISTER kare -> profile overwrite -> score 0, tag saaf.    │
//  │  Dobara wahi khel. Aur `updateProfile` me bhi `setPrimaryTag(...)` seedha │
//  │  SPAM badge hata deta tha.                                               │
//  │                                                                          │
//  │  CONFIRM: score 70 -> registerUser(us number se) -> score 0. ✅ bug       │
//  │                                                                          │
//  │  FIX: dono jagah ab `PhoneProfile::claimOwnership()` use hota hai — wo    │
//  │  naam/owner/tag update karta hai par `spamScore_`/`reportCount_` ko haath │
//  │  nahi lagata, aur number pehle se spam ho to tag bhi SPAM hi rakhta hai.  │
//  │  📌 RULE: user apna DATA badal sakta hai, DOOSRON KA FAISLA nahi.        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ MEMBER ORDER: `spamStrategy_` `spamReportService_` se PEHLE declare hai —
//     kyunki constructor usi se `spamReportService_` banata hai. C++ me members
//     DECLARATION ke order me bante hain (init-list ke order me nahi). Ulta
//     hota to service ko aisa strategy milta jo abhi bana hi nahi. 😱
//
//  📌 Copy/move yahan SAFE hai (kai doosre LLDs ke ulat) — kyunki koi bhi
//     service sibling member ka REFERENCE nahi rakhti. `SpamReportService`
//     `shared_ptr` by-value rakhti hai, aur baaki services stateless hain aur
//     data PARAMETER se leti hain. Isliye yahan `= delete` ki zaroorat nahi.
// ============================================================================
#ifndef TRUECALLER_LLD_CORE_TRUECALLERSYSTEM_H
#define TRUECALLER_LLD_CORE_TRUECALLERSYSTEM_H

#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "../enums/AccountStatus.h"
#include "../enums/CallType.h"
#include "../enums/CallerTag.h"
#include "../enums/ReportReason.h"
#include "../models/CallLogEntry.h"
#include "../models/CallerLookupResult.h"
#include "../models/ContactEntry.h"
#include "../models/PhoneProfile.h"
#include "../models/User.h"
#include "../services/BlockService.h"
#include "../services/CallLogService.h"
#include "../services/ContactSyncService.h"
#include "../services/LookupService.h"
#include "../services/SearchService.h"
#include "../services/SpamReportService.h"
#include "../strategies/DefaultSpamScoringStrategy.h"
#include "../strategies/ISpamScoringStrategy.h"
#include "../utils/PhoneUtils.h"

namespace truecaller_lld {

class TruecallerSystem {
public:
    // Wiring point: default strategy banao aur spam service me inject karo.
    TruecallerSystem()
        : spamStrategy_(std::make_shared<DefaultSpamScoringStrategy>()),
          spamReportService_(spamStrategy_) {}

    // ---- Registration ------------------------------------------------------
    std::string registerUser(const std::string& displayName, const std::string& phone,
                             const std::string& email = "", bool isBusiness = false) {
        const std::string normalized = normalizePhone(phone);
        // ⭐ Ek number = ek account. Ye map hi unique constraint hai.
        if (phoneToUserId_.count(normalized) != 0) {
            throw std::runtime_error("phone already registered: " + normalized);
        }
        if (displayName.empty()) {
            throw std::invalid_argument("display name required");
        }

        const std::string userId = "TC_U" + std::to_string(++userCounter_);
        User user(userId, displayName, normalized, email, isBusiness);
        users_.emplace(userId, std::move(user));
        phoneToUserId_[normalized] = userId;

        const CallerTag tag = isBusiness ? CallerTag::BUSINESS : CallerTag::PERSONAL;
        upsertDirectoryProfile(normalized, userId, displayName, tag); // ⭐ fix (neeche)
        return userId;
    }

    // ⭐ DONO jagah update — warna directory me purana naam pada rehta.
    void updateProfile(const std::string& userId, const std::string& displayName,
                       const std::string& email, bool isBusiness) {
        User& user = getUserOrThrow(userId); // ye SUSPENDED pe throw karta hai
        user.setDisplayName(displayName);    // khaali naam ka check User me hai
        user.setEmail(email);
        user.setBusiness(isBusiness);

        PhoneProfile& profile = getDirectoryProfileOrThrow(user.getPhone());
        // ⭐⭐ BUG FIX: pehle yahan setDisplayName + setPrimaryTag seedha lagta
        // tha — jisse SPAM badge hat jaata tha. Ab claimOwnership spam history
        // ko surakshit rakhta hai (upar poora note).
        profile.claimOwnership(userId, displayName,
                               isBusiness ? CallerTag::BUSINESS : CallerTag::PERSONAL);
    }

    // ---- ⭐ Main feature: CALLER ID ---------------------------------------
    CallerLookupResult identifyCaller(const std::string& forUserId, const std::string& callerPhone) {
        validateUser(forUserId);
        // Global directory + is user ki personal blocklist — dono jud ke jawab.
        return lookupService_.lookup(forUserId, callerPhone, directory_, blockService_);
    }

    // ---- Contact sync ------------------------------------------------------
    std::vector<ContactEntry> syncContacts(const std::string& userId,
                                           const std::vector<ContactEntry>& contacts) {
        validateUser(userId);
        // Enriched result store bhi hota hai (baad me dobara sync na karna pade).
        syncedContacts_[userId] = contactSyncService_.syncContacts(contacts, directory_);
        return syncedContacts_[userId];
    }

    // ---- Spam ---------------------------------------------------------------
    void reportSpam(const std::string& reporterUserId, const std::string& phone,
                    ReportReason reason) {
        validateUser(reporterUserId); // sirf registered log report kar sakte hain
        // ⭐ Note: reported number ka registered hona ZAROORI NAHI — service
        // profile khud bana leti hai (crowd-sourced directory).
        spamReportService_.reportSpam(reporterUserId, phone, reason, directory_);
    }

    // ---- Block (personal) --------------------------------------------------
    void blockNumber(const std::string& userId, const std::string& phone) {
        validateUser(userId);
        blockService_.block(userId, phone);
    }

    void unblockNumber(const std::string& userId, const std::string& phone) {
        validateUser(userId);
        blockService_.unblock(userId, phone);
    }

    std::vector<std::string> getBlockedNumbers(const std::string& userId) const {
        validateUser(userId);
        return blockService_.listBlocked(userId); // sorted (deterministic)
    }

    // ---- Search ------------------------------------------------------------
    std::vector<PhoneProfile> searchByName(const std::string& prefix, size_t limit = 10) const {
        if (prefix.empty()) {
            // ⭐ Khaali prefix = "sab kuch match" = poori directory dump.
            // Privacy ke liye ye block hona hi chahiye.
            throw std::invalid_argument("search prefix required");
        }
        return searchService_.searchByNamePrefix(prefix, directory_, limit);
    }

    std::vector<PhoneProfile> searchByPhone(const std::string& phone) const {
        return searchService_.searchByPhone(phone, directory_);
    }

    // ---- Call log ----------------------------------------------------------
    std::string logCall(const std::string& userId, const std::string& remotePhone, CallType type) {
        validateUser(userId);
        return callLogService_.logCall(userId, remotePhone, type);
    }

    std::vector<CallLogEntry> getCallHistory(const std::string& userId, size_t limit = 20) const {
        validateUser(userId);
        return callLogService_.getRecentCalls(userId, limit); // newest pehle
    }

    const User& getUser(const std::string& userId) const { return getUserOrThrow(userId); }

private:
    // ---- Data (facade hi maalik hai) ---------------------------------------
    std::unordered_map<std::string, User> users_;                // userId -> account
    std::unordered_map<std::string, std::string> phoneToUserId_; // index + unique constraint
    std::unordered_map<std::string, PhoneProfile> directory_;    // ⭐ GLOBAL directory (core)
    std::unordered_map<std::string, std::vector<ContactEntry>> syncedContacts_;

    // ---- Services (stateless — data inhe parameters se milta hai) ----------
    BlockService blockService_;
    LookupService lookupService_;
    ContactSyncService contactSyncService_;
    SearchService searchService_;
    CallLogService callLogService_;
    // ⭐ ORDER MATTERS: strategy pehle, phir usko use karne wali service.
    std::shared_ptr<ISpamScoringStrategy> spamStrategy_;
    SpamReportService spamReportService_;

    int userCounter_{0};

    // ---- Private helpers ---------------------------------------------------

    // ⭐⭐ BUG FIX ka dil: profile pehle se ho to OVERWRITE mat karo —
    // sirf ownership claim karo (spam score/report count bache rahein).
    void upsertDirectoryProfile(const std::string& phone, const std::string& userId,
                                const std::string& displayName, CallerTag tag) {
        auto it = directory_.find(phone);
        if (it == directory_.end()) {
            // Bilkul naya number — normal profile bana do.
            directory_.emplace(phone, PhoneProfile(phone, displayName, userId, tag, 0));
            return;
        }
        // Pehle se profile hai (matlab kisi ne report kiya tha) —
        // reports SURAKSHIT rakho, sirf identity update karo. 🔒
        it->second.claimOwnership(userId, displayName, tag);
    }

    // ⚠ Sirf existence check — SUSPENDED status yahan NAHI dekha jaata
    //    (AccountStatus.h me is adhoori wiring ka note hai).
    void validateUser(const std::string& userId) const {
        if (users_.find(userId) == users_.end()) {
            throw std::runtime_error("user not found: " + userId);
        }
    }

    // Non-const (write path): yahan SUSPENDED bhi block hota hai.
    User& getUserOrThrow(const std::string& userId) {
        auto it = users_.find(userId);
        if (it == users_.end()) {
            throw std::runtime_error("user not found: " + userId);
        }
        if (it->second.getStatus() != AccountStatus::ACTIVE) {
            throw std::runtime_error("user account suspended");
        }
        return it->second;
    }

    // Const (read path) — status check nahi (read-only ko rokna policy call hai).
    const User& getUserOrThrow(const std::string& userId) const {
        auto it = users_.find(userId);
        if (it == users_.end()) {
            throw std::runtime_error("user not found: " + userId);
        }
        return it->second;
    }

    PhoneProfile& getDirectoryProfileOrThrow(const std::string& phone) {
        auto it = directory_.find(phone);
        if (it == directory_.end()) {
            // Registered user ka profile hamesha hona chahiye — na hona
            // data inconsistency hai, isliye saaf error.
            throw std::runtime_error("directory profile missing for " + phone);
        }
        return it->second;
    }
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_CORE_TRUECALLERSYSTEM_H

// ============================================================================
//  services/BlockService.h  —  Per-user blocklist
// ----------------------------------------------------------------------------
//  Storage: `unordered_map<userId, unordered_set<phone>>`
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Ye do-level structure kyun (`map<user, set<phone>>`)?                 │
//  │                                                                          │
//  │  Kyunki block PERSONAL hai, GLOBAL nahi. Alice ne block kiya to Bob ko    │
//  │  us number se call aati rahegi — aur ye bilkul sahi hai.                 │
//  │  (Spam score iska ulta hai — wo GLOBAL hai. Dono ka farak                │
//  │   `CallerLookupResult.h` me detail me likha hai.)                        │
//  │                                                                          │
//  │  ⭐ Andar `unordered_set` (vector nahi):                                  │
//  │     • `isBlocked` O(1) — ye har incoming call pe chalta hai, sabse HOT    │
//  │       path hai. Vector me O(N) scan hota.                                │
//  │     • duplicate apne aap block — do baar block karo, ek hi entry.        │
//  │       Yaani `block()` naturally IDEMPOTENT hai.                          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `blocked_[userId].insert(...)` — `operator[]` user ki entry NAHI hone pe
//     apne aap khaali set bana deta hai. Isliye "pehle user register karo, phir
//     block" jaisa koi setup nahi chahiye. (Lazy creation.)
//
//  ⭐ `unblock` chup-chaap return karta hai agar user/number mila hi nahi —
//     throw nahi karta. Kyunki "jo block hai hi nahi usko unblock karna" ka
//     natija wahi hai jo user chahta tha: number blocked nahi hai. ✅
//     📌 Ye IDEMPOTENT design hai — dobara call karo, kuch nahi bigadta.
//
//  ⭐ `normalizePhone` TEENO methods me hai — ye critical hai. Alice ne
//     "+91 98765 43210" se block kiya aur call "9876543210" se aayi, to bina
//     normalize kiye block kaam hi nahi karta. 😱 Ek hi canonical form dono
//     taraf hone se ye hamesha match karta hai. (PhoneUtils.h ka poora note.)
//
//  ⭐ `listBlocked` SORT karke deta hai — `unordered_set` ka traversal order
//     non-deterministic hota hai, to bina sort ke UI me numbers ka order har
//     baar badal jaata (aur tests flaky ho jaate).
//
//  ⚠ Ek honest gap: block ka lookup key `phone` hai, `userId` nahi. Yaani
//     spammer number badal ke phir call kar sakta hai. Asli app me device
//     fingerprint / operator-level blocking hoti hai — par wo HLD ka scope hai.
// ============================================================================
#ifndef TRUECALLER_LLD_SERVICES_BLOCKSERVICE_H
#define TRUECALLER_LLD_SERVICES_BLOCKSERVICE_H

#include <algorithm>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../utils/PhoneUtils.h"

namespace truecaller_lld {

class BlockService {
public:
    // set hai -> dobara block karna no-op (naturally idempotent).
    void block(const std::string& userId, const std::string& phone) {
        blocked_[userId].insert(normalizePhone(phone)); // operator[] lazy set banata hai
    }

    // ⭐ Nahi mila to chup-chaap return (idempotent — upar note).
    void unblock(const std::string& userId, const std::string& phone) {
        auto it = blocked_.find(userId);
        if (it == blocked_.end()) {
            return;
        }
        it->second.erase(normalizePhone(phone));
    }

    // ⭐ HOT PATH — har incoming call pe chalta hai. O(1) hona zaroori hai.
    bool isBlocked(const std::string& userId, const std::string& phone) const {
        auto it = blocked_.find(userId);
        if (it == blocked_.end()) {
            return false; // is user ne kabhi kuch block hi nahi kiya
        }
        return it->second.count(normalizePhone(phone)) != 0;
    }

    std::vector<std::string> listBlocked(const std::string& userId) const {
        std::vector<std::string> result;
        auto it = blocked_.find(userId);
        if (it == blocked_.end()) {
            return result; // khaali list (error nahi — normal case)
        }
        result.assign(it->second.begin(), it->second.end());
        // ⭐ unordered_set ka order random hota hai -> sort karke deterministic banao
        std::sort(result.begin(), result.end());
        return result;
    }

private:
    // ⭐ userId -> uske blocked numbers (PERSONAL data, global nahi)
    std::unordered_map<std::string, std::unordered_set<std::string>> blocked_;
};

}  // namespace truecaller_lld

#endif  // TRUECALLER_LLD_SERVICES_BLOCKSERVICE_H

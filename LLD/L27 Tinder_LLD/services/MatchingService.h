// ============================================================================
//  services/MatchingService.h — nearby profile DISCOVERY (service layer)
// ----------------------------------------------------------------------------
//  Ye service "discovery" logic ko domain models se ALAG rakhta hai
//  (separation of concerns). findNearbyProfiles() ka kaam: user ke liye
//  wo candidates dhundho jo:
//    1. Khud nahi (self skip)
//    2. Pehle se interact NAHI kiye (already swiped ko dobara nahi dikhana)
//    3. maxDistanceKm ke ANDAR hain (Haversine distance se)
//
//  ⭐ "interacted" check hi Tinder ka "already seen" behavior deta — jise
//  ek baar swipe kar diya wo card dobara nahi aata. Real Tinder me bhi
//  aisa hi (swiped profiles wapas nahi aate).
//
//  DHYAN DO — ye alag class kyun (User me ye method daal sakte the)?
//  Kyunki discovery cross-user operation hai (saare users pe iterate) —
//  ye ek USER ka kaam nahi, SYSTEM ka kaam hai. Isliye service layer.
//  Monolith version me ye "Matcher" Strategy pattern se hota hai (multiple
//  matching algorithms) — ye modular version simpler distance-based hai.
// ============================================================================
#ifndef TINDER_LLD_SERVICES_MATCHINGSERVICE_H
#define TINDER_LLD_SERVICES_MATCHINGSERVICE_H

#include <bits/stdc++.h>

#include "../models/User.h"

using namespace std;

namespace tinder_lld {

class MatchingService {
public:
    vector<User *> findNearbyProfiles(User *user, const unordered_map<string, User *> &allUsers, double maxDistanceKm) const {
        vector<User *> results;
        for (const auto &entry : allUsers) {
            User *candidate = entry.second;
            if (candidate->getUserId() == user->getUserId()) {
                continue;
            }
            if (user->interacted(candidate->getUserId())) {
                continue;
            }
            const double distance = user->getProfile().getLocation().distanceInKm(candidate->getProfile().getLocation());
            if (distance <= maxDistanceKm) {
                results.push_back(candidate);
            }
        }
        return results;
    }
};

} // namespace tinder_lld

#endif // TINDER_LLD_SERVICES_MATCHINGSERVICE_H

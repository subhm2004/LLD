// services/MatchingService.h — Core matching logic: swipes record karta hai,
// MUTUAL right-swipe pe match banata hai, aur match hone par ChatRoom create
// karta hai. Domain models se alag, orchestration yahan.
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

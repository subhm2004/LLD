// ============================================================================
//  models/User.h — App user: id + profile + swipe history + activity
// ----------------------------------------------------------------------------
//  Ek user ka core: userId, profile (naam/age/gender/location), aur
//  swipeHistory_ (kisko kya swipe kiya). lastActive_ activity tracking.
//
//  ⭐ swipeHistory_ (map: targetId -> SwipeAction) hi MATCH detection ka
//  aadhaar hai:
//    liked(x)      -> "maine x ko RIGHT/SUPER_LIKE kiya?" (match check)
//    interacted(x) -> "maine x ko koi bhi swipe kiya?" (discovery me skip)
//  Jab A swipe kare B pe, system check karta B->liked(A)? Agar haan =
//  MUTUAL like = MATCH! (TinderSystem.swipe me ye logic hai.)
//
//  undoSwipe() -> swipe history se hata do (Tinder ka "rewind" feature).
//  touch() -> lastActive update (koi bhi activity pe "last seen" refresh).
//  Ownership: User apni profile + history ka malik (value members).
// ============================================================================
#ifndef TINDER_LLD_MODELS_USER_H
#define TINDER_LLD_MODELS_USER_H

#include <bits/stdc++.h>

#include "../enums/SwipeAction.h"
#include "UserProfile.h"

using namespace std;

namespace tinder_lld {

class User {
public:
    User(string userId, UserProfile profile)
        : userId_(std::move(userId)), profile_(std::move(profile)), lastActive_(time(nullptr)) {}

    const string &getUserId() const { return userId_; }
    const UserProfile &getProfile() const { return profile_; }

    void swipe(const string &targetUserId, SwipeAction action) {
        swipeHistory_[targetUserId] = action;
        touch();
    }

    bool undoSwipe(const string &targetUserId) {
        auto erased = swipeHistory_.erase(targetUserId);
        if (erased > 0) {
            touch();
            return true;
        }
        return false;
    }

    bool liked(const string &targetUserId) const {
        auto it = swipeHistory_.find(targetUserId);
        return it != swipeHistory_.end() &&
               (it->second == SwipeAction::RIGHT || it->second == SwipeAction::SUPER_LIKE);
    }

    bool interacted(const string &targetUserId) const { return swipeHistory_.count(targetUserId) != 0; }
    long long getLastActive() const { return lastActive_; }
    void touch() { lastActive_ = time(nullptr); }

private:
    string userId_;
    UserProfile profile_;
    unordered_map<string, SwipeAction> swipeHistory_;
    long long lastActive_;
};

} // namespace tinder_lld

#endif // TINDER_LLD_MODELS_USER_H

// ============================================================================
//  models/User.h  —  Ek user + uska FOLLOW graph
// ----------------------------------------------------------------------------
//  User ke paas apni id, naam, aur "main kin logon ko follow karta hoon" ki list
//  hoti hai. Yahi follow-graph feed banane ka aadhaar hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ FOLLOW ONE-WAY (directional) hai — dosti (mutual) nahi                │
//  │                                                                          │
//  │  `following_` sirf "main kinko follow karta hoon" rakhta hai. Agar U1     │
//  │  U2 ko follow kare, iska matlab NAHI ki U2 bhi U1 ko follow karta hai —   │
//  │  Instagram/YouTube jaisa one-way model (LinkedIn ki mutual "connection"   │
//  │  se alag). Isi liye ek hi set kaafi, dono taraf update nahi karte.        │
//  │                                                                          │
//  │  Feed isi se banti: "jinko main follow karta hoon unke reels + mere apne".│
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `unordered_set` (vector nahi) — do fayde:
//     1. `isFollowing()` O(1) me (feed banate waqt har reel pe ye check hota)
//     2. Duplicate follow apne aap block (set me same id do baar nahi jaati)
//
//  ⭐ SELF-FOLLOW guard: `if (userId != id_)` — koi khud ko follow na kar sake.
//     (Warna feed me apne reels do baar aate — ek "following" se, ek "own" se.)
// ============================================================================
#ifndef INSTA_REEL_LLD_MODELS_USER_H
#define INSTA_REEL_LLD_MODELS_USER_H

#include <string>
#include <unordered_set>
#include <utility>

using namespace std;

namespace reel_platform_lld {

class User {
public:
    User(string id, string name) : id_(std::move(id)), name_(std::move(name)) {}

    const string &getId() const { return id_; }
    const string &getName() const { return name_; }

    // Kisi ko follow karo (khud ko nahi — upar note).
    void follow(const string &userId) {
        if (userId != id_) {
            following_.insert(userId); // set -> duplicate apne aap ignore
        }
    }

    // Feed banate waqt har reel ke owner pe ye O(1) check chalta hai.
    bool isFollowing(const string &userId) const { return following_.count(userId) != 0; }

private:
    string id_;                        // "U1" — unique
    string name_;                      // "Shubham"
    unordered_set<string> following_;  // ⭐ one-way follow graph (main kinko follow karta hoon)
};

} // namespace reel_platform_lld

#endif // INSTA_REEL_LLD_MODELS_USER_H

// ============================================================================
//  main.cpp  —  TINDER-style dating app ka demo driver (L27 LLD project)
// ----------------------------------------------------------------------------
//  System kya karta hai: users register karo (profile + location) -> nearby
//  profiles discover karo (X km ke andar) -> swipe (left/right/super-like) ->
//  MUTUAL like pe MATCH + chat room -> messages + read receipts. Plus daily
//  swipe limit, undo swipe, block/unmatch. Poora Tinder ka mini LLD!
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  MATCH KAISE BANTA HAI (core logic):                                    │
//  │                                                                          │
//  │   user1 --RIGHT--> user2   (user1 ne user2 ko like kiya)               │
//  │   user2 --RIGHT--> user1   (user2 ne bhi like kiya)                    │
//  │        │                                                                │
//  │        ▼ MUTUAL like! -> MATCH -> ChatRoom ban gaya                     │
//  │        │                                                                │
//  │   ab dono chat kar sakte hain (sirf matched users chat kar sakte)      │
//  │                                                                          │
//  │  Agar user2 ne LEFT (reject) kiya hota to match NAHI hota — chat        │
//  │  room banta hi nahi. "Mutual interest" hi Tinder ka pura funda hai!    │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  IS MODULAR VERSION KE PATTERNS:
//    Facade    -> TinderSystem (registration/discover/swipe/chat ek API se)
//    Service   -> MatchingService (nearby-profile logic alag layer me)
//  (Monolith version C++ Code/Tinder_LLD.cpp me ZYADA patterns hain:
//   Observer + Strategy + Factory + Singleton — detail: design_patterns_used.md)
//
//  DEMO FLOW: 2 users register -> discover -> super-like + undo -> mutual
//  right-swipe (MATCH!) -> chat -> read messages -> unmatch -> block.
// ============================================================================
#include <bits/stdc++.h>

#include "core/TinderSystem.h"

using namespace std;
using namespace tinder_lld;

int main() {
    TinderSystem app;

    app.registerUser("user1", UserProfile("Rohan", 28, Gender::MALE, {28.60, 77.20}));
    app.registerUser("user2", UserProfile("Neha", 27, Gender::FEMALE, {28.61, 77.21}));

    vector<User *> profiles = app.discoverProfiles("user1", 5.0);
    cout << "Nearby profiles for user1: " << profiles.size() << "\n";
    for (User *u : profiles) {
        cout << "- " << u->getProfile().getName() << "\n";
    }

    app.swipe("user1", "user2", SwipeAction::SUPER_LIKE);
    app.undoSwipe("user1", "user2");
    app.swipe("user1", "user2", SwipeAction::RIGHT);
    app.swipe("user2", "user1", SwipeAction::RIGHT);

    app.sendMessage("user1", "user2", "Hi Neha!");
    app.sendMessage("user2", "user1", "Hi Rohan, nice to connect!");
    app.readMessages("user2", "user1");
    app.printChat("user1", "user2");
    cout << "Last active user1: " << app.getLastActive("user1") << "\n";
    cout << "Last active user2: " << app.getLastActive("user2") << "\n";

    app.unmatch("user1", "user2");
    app.blockUser("user1", "user2");
    return 0;
}

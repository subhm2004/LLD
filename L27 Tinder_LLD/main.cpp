// ============================================================================
//  main.cpp  —  Tinder-style dating app ka demo driver
// ----------------------------------------------------------------------------
//  Users register karo (profile + location), nearby profiles discover karo,
//  swipe (left/right/super-like), mutual like pe match + chat. Daily swipe limit
//  aur block/unmatch bhi. Sab TinderSystem facade ke through.
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

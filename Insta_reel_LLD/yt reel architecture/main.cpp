// ============================================================================
//  main.cpp  —  Reel platform ka demo driver (CLIENT)
// ----------------------------------------------------------------------------
//  Poora flow: users register -> follow -> reels upload -> engagement (view/like/
//  comment) -> personalized feed.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ FEED ka expected result (ranking samajhne ke liye)                    │
//  │                                                                          │
//  │  U1 follow karta hai U2 aur U3 ko. To feed me aayenge:                   │
//  │     REEL_1 (U2 ka), REEL_2 (U3 ka), REEL_3 (U1 ka apna)                  │
//  │                                                                          │
//  │  Ranking (views DESC, tie pe likes DESC):                                │
//  │     REEL_2 -> 1 view, 1 like  -> pehla (views barabar par likes zyada)    │
//  │     REEL_1 -> 1 view, 0 like  -> doosra                                   │
//  │     REEL_3 -> 0 view          -> teesra                                   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ Client ko sirf `ReelPlatformService` dikhta hai (Facade) — follow-graph,
//     ranking, storage kuch nahi. Bas saaf methods.
//
//  📌 Includes targeted (bits/stdc++.h nahi) — portable + saaf.
// ============================================================================
#include <iostream>
#include <string>
#include <vector>

#include "services/ReelPlatformService.h"

using namespace std;
using namespace reel_platform_lld;

int main() {
    ReelPlatformService reelService;

    // ---- Users register ----------------------------------------------------
    reelService.registerUser("U1", "Shubham");
    reelService.registerUser("U2", "Ananya");
    reelService.registerUser("U3", "Ravi");

    // ---- Follow (one-way): U1 -> U2, U1 -> U3 ------------------------------
    reelService.follow("U1", "U2");
    reelService.follow("U1", "U3");

    // ---- Reels upload (har user ka apna) -----------------------------------
    string reel1 = reelService.uploadReel("U2", "Sunset time-lapse in 4K", 24);
    string reel2 = reelService.uploadReel("U3", "Quick C++ memory tips", 35);
    string reel3 = reelService.uploadReel("U1", "LLD prep workflow", 42);

    // ---- Engagement: view / like / comment ---------------------------------
    reelService.viewReel("U1", reel1);              // REEL_1: 1 view
    reelService.viewReel("U1", reel2);              // REEL_2: 1 view
    reelService.likeReel("U1", reel2);              // REEL_2: 1 like (tie-breaker jeetega)
    reelService.commentOnReel("U1", reel3, "Great flow!"); // REEL_3: 1 comment

    // ---- Personalized feed (top 5, engagement-ranked) ---------------------
    vector<Reel *> feed = reelService.getFeed("U1", 5);

    cout << "=== Feed for U1 ===\n";
    for (const auto &reel : feed) {
        reel->printStats();
    }

    return 0;
    // ⭐ Koi delete nahi — reelService stack pe hai, scope khatam hote hi uska
    //    destructor saare User*/Reel* saaf kar dega.
}

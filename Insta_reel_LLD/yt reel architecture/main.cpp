#include <bits/stdc++.h>

#include "services/ReelPlatformService.h"

using namespace std;
using namespace reel_platform_lld;

int main() {
    ReelPlatformService reelService;

    reelService.registerUser("U1", "Shubham");
    reelService.registerUser("U2", "Ananya");
    reelService.registerUser("U3", "Ravi");

    reelService.follow("U1", "U2");
    reelService.follow("U1", "U3");

    string reel1 = reelService.uploadReel("U2", "Sunset time-lapse in 4K", 24);
    string reel2 = reelService.uploadReel("U3", "Quick C++ memory tips", 35);
    string reel3 = reelService.uploadReel("U1", "LLD prep workflow", 42);

    reelService.viewReel("U1", reel1);
    reelService.viewReel("U1", reel2);
    reelService.likeReel("U1", reel2);
    reelService.commentOnReel("U1", reel3, "Great flow!");

    vector<Reel *> feed = reelService.getFeed("U1", 5);

    cout << "=== Feed for U1 ===\n";
    for (const auto &reel : feed) {
        reel->printStats();
    }

    return 0;
}

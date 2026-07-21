// ============================================================================
//  services/ReelPlatformService.h  —  Poore platform ka FACADE (single entry)
// ----------------------------------------------------------------------------
//  Ye class hi client ke liye "Instagram Reels" hai. Users, reels, follow-graph,
//  aur feed generation — sab yahi orchestrate karta hai. Client ko sirf saaf
//  methods dikhte hain: registerUser, follow, uploadReel, view/like/comment, getFeed.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ FACADE / SERVICE LAYER — client ko internals se bachao               │
//  │                                                                          │
//  │  Client ko ye NAHI pata: users/reels kaise store hote, follow-graph kaise │
//  │  check hota, feed kaise rank hoti. Wo bas method bulata hai; service      │
//  │  validate karke sahi model ko delegate kar deta.                         │
//  │                                                                          │
//  │  Notice: engagement ka LOGIC models me hai (Reel::addLike duplicate       │
//  │  handle karta), service sirf VALIDATE + delegate karta. Ye accha SRP hai  │
//  │  — service patla, models rich.                                          │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ FEED GENERATION (FR-7, FR-8) — 3 steps                                │
//  │                                                                          │
//  │   1. FILTER : wo reels jinke owner ko main follow karta hoon, YA mere apne│
//  │   2. RANK   : views DESC, tie ho to likes DESC (popularity feed)          │
//  │   3. LIMIT  : top N kaat do                                              │
//  │                                                                          │
//  │  ⚠ Ye O(N) scan hai saare reels pe (N = platform ke total reels). Real    │
//  │  system me per-user precomputed timeline (fan-out on write) hota. Demo    │
//  │  scale pe theek — interview me ye trade-off bolna accha point hai.       │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⚠ MEMORY: users_/reels_ raw pointers (`new`) hain, destructor delete    │
//  │     karta hai. ISI LIYE COPY/MOVE EXPLICITLY DELETE KIYE HAIN:            │
//  │                                                                          │
//  │  🐛 Ye ek ASLI latent bug thi: is class ke saare members copyable hain    │
//  │     (2 pointer-maps + ek vector-map + int) — koi unique_ptr nahi jo copy  │
//  │     rok le. To compiler ka IMPLICIT copy constructor pointers ko SHALLOW  │
//  │     copy karta: do copies same User*/Reel* pakde hue -> dono ke destructor│
//  │     usi pointer ko delete karte -> DOUBLE FREE 💥 (crash).                │
//  │                                                                          │
//  │  ✅ Fix: copy/move `= delete` — ab galti se copy compile-error degi,      │
//  │     silent crash nahi. (Rule of Three/Five: destructor likha to copy/move │
//  │     ka bhi socho.) 💡 Aur behtar: `unordered_map<string, unique_ptr<...>>`│
//  │     ya by-value storage — tab manual delete hi na padta.                 │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef INSTA_REEL_LLD_SERVICES_REELPLATFORMSERVICE_H
#define INSTA_REEL_LLD_SERVICES_REELPLATFORMSERVICE_H

#include <algorithm>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../models/Reel.h"
#include "../models/User.h"

using namespace std;

namespace reel_platform_lld {

class ReelPlatformService {
public:
    ReelPlatformService() = default;

    // Heap pe bane users/reels saaf karo (koi leak na rahe).
    ~ReelPlatformService() {
        for (auto &entry : users_) {
            delete entry.second;
        }
        for (auto &entry : reels_) {
            delete entry.second;
        }
    }

    // ⭐ Copy/move BAND — warna raw pointers shallow-copy hoke DOUBLE FREE (upar note).
    ReelPlatformService(const ReelPlatformService &) = delete;
    ReelPlatformService &operator=(const ReelPlatformService &) = delete;
    ReelPlatformService(ReelPlatformService &&) = delete;
    ReelPlatformService &operator=(ReelPlatformService &&) = delete;

    // ---- FR-1: user register (duplicate id reject) -------------------------
    void registerUser(const string &userId, const string &name) {
        if (users_.count(userId) != 0) {
            throw runtime_error("User already exists: " + userId);
        }
        users_[userId] = new User(userId, name);
    }

    // ---- FR-2: follow (dono users exist karne chahiye) ---------------------
    void follow(const string &followerId, const string &followeeId) {
        User *follower = getUserOrThrow(followerId);
        getUserOrThrow(followeeId); // followee bhi valid ho (validate only)
        follower->follow(followeeId); // one-way edge (User.h note dekho)
    }

    // ---- FR-3: reel upload -> unique id generate ---------------------------
    string uploadReel(const string &ownerId, const string &caption, int durationSec) {
        getUserOrThrow(ownerId);
        const string reelId = "REEL_" + to_string(++reelCounter_);
        reels_[reelId] = new Reel(reelId, ownerId, caption, durationSec);
        userToReels_[ownerId].push_back(reelId); // reverse index: "user ke reels" (future queries)
        return reelId;
    }

    // ---- FR-4/5/6: view / like / unlike / comment (models ko delegate) -----
    // Service sirf VALIDATE karti hai; asli logic (duplicate-like etc.) Reel me hai.
    void viewReel(const string &viewerId, const string &reelId) {
        getUserOrThrow(viewerId);
        getReelOrThrow(reelId)->addView(viewerId);
    }

    void likeReel(const string &userId, const string &reelId) {
        getUserOrThrow(userId);
        getReelOrThrow(reelId)->addLike(userId); // duplicate Reel khud handle karta
    }

    void unlikeReel(const string &userId, const string &reelId) {
        getUserOrThrow(userId);
        getReelOrThrow(reelId)->removeLike(userId);
    }

    void commentOnReel(const string &userId, const string &reelId, const string &text) {
        getUserOrThrow(userId);
        getReelOrThrow(reelId)->addComment(userId, text);
    }

    // ========================================================================
    //  ⭐ FR-7/8: PERSONALIZED FEED — filter -> rank -> limit
    // ========================================================================
    vector<Reel *> getFeed(const string &userId, size_t limit = 10) {
        User *user = getUserOrThrow(userId);
        vector<Reel *> feed;

        // 1. FILTER: jinko follow karta hoon unke reels + apne reels
        for (const auto &entry : reels_) {
            Reel *reel = entry.second;
            if (user->isFollowing(reel->getOwnerId()) || reel->getOwnerId() == userId) {
                feed.push_back(reel);
            }
        }

        // 2. RANK: views zyada -> upar; barabar ho to likes zyada -> upar
        sort(feed.begin(), feed.end(), [](const Reel *a, const Reel *b) {
            if (a->getTotalViews() == b->getTotalViews()) {
                return a->getTotalLikes() > b->getTotalLikes(); // tie-breaker
            }
            return a->getTotalViews() > b->getTotalViews();
        });

        // 3. LIMIT: top N (simple pagination)
        if (feed.size() > limit) {
            feed.resize(limit);
        }
        return feed;
    }

private:
    // "get-or-throw" helpers — galat id pe saaf error (fail-fast, silent null nahi).
    User *getUserOrThrow(const string &userId) {
        auto it = users_.find(userId);
        if (it == users_.end()) {
            throw runtime_error("User not found: " + userId);
        }
        return it->second;
    }

    Reel *getReelOrThrow(const string &reelId) {
        auto it = reels_.find(reelId);
        if (it == reels_.end()) {
            throw runtime_error("Reel not found: " + reelId);
        }
        return it->second;
    }

    unordered_map<string, User *> users_;              // userId -> User (OWNER — dtor deletes)
    unordered_map<string, Reel *> reels_;              // reelId -> Reel (OWNER — dtor deletes)
    unordered_map<string, vector<string>> userToReels_; // reverse index: user ke apne reels
    int reelCounter_ = 0;                              // unique reel id ke liye
};

} // namespace reel_platform_lld

#endif // INSTA_REEL_LLD_SERVICES_REELPLATFORMSERVICE_H

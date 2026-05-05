#ifndef INSTA_REEL_LLD_SERVICES_REELPLATFORMSERVICE_H
#define INSTA_REEL_LLD_SERVICES_REELPLATFORMSERVICE_H

#include <bits/stdc++.h>

#include "../models/Reel.h"
#include "../models/User.h"

using namespace std;

namespace reel_platform_lld {

class ReelPlatformService {
public:
    ~ReelPlatformService() {
        for (auto &entry : users_) {
            delete entry.second;
        }
        for (auto &entry : reels_) {
            delete entry.second;
        }
    }

    void registerUser(const string &userId, const string &name) {
        if (users_.count(userId) != 0) {
            throw runtime_error("User already exists: " + userId);
        }
        users_[userId] = new User(userId, name);
    }

    void follow(const string &followerId, const string &followeeId) {
        User *follower = getUserOrThrow(followerId);
        getUserOrThrow(followeeId);
        follower->follow(followeeId);
    }

    string uploadReel(const string &ownerId, const string &caption, int durationSec) {
        getUserOrThrow(ownerId);
        const string reelId = "REEL_" + to_string(++reelCounter_);
        reels_[reelId] = new Reel(reelId, ownerId, caption, durationSec);
        userToReels_[ownerId].push_back(reelId);
        return reelId;
    }

    void viewReel(const string &viewerId, const string &reelId) {
        getUserOrThrow(viewerId);
        Reel *reel = getReelOrThrow(reelId);
        reel->addView(viewerId);
    }

    void likeReel(const string &userId, const string &reelId) {
        getUserOrThrow(userId);
        Reel *reel = getReelOrThrow(reelId);
        reel->addLike(userId);
    }

    void unlikeReel(const string &userId, const string &reelId) {
        getUserOrThrow(userId);
        Reel *reel = getReelOrThrow(reelId);
        reel->removeLike(userId);
    }

    void commentOnReel(const string &userId, const string &reelId, const string &text) {
        getUserOrThrow(userId);
        Reel *reel = getReelOrThrow(reelId);
        reel->addComment(userId, text);
    }

    vector<Reel *> getFeed(const string &userId, size_t limit = 10) {
        User *user = getUserOrThrow(userId);
        vector<Reel *> feed;

        for (const auto &entry : reels_) {
            Reel *reel = entry.second;
            if (user->isFollowing(reel->getOwnerId()) || reel->getOwnerId() == userId) {
                feed.push_back(reel);
            }
        }

        sort(feed.begin(), feed.end(), [](const Reel *a, const Reel *b) {
            if (a->getTotalViews() == b->getTotalViews()) {
                return a->getTotalLikes() > b->getTotalLikes();
            }
            return a->getTotalViews() > b->getTotalViews();
        });

        if (feed.size() > limit) {
            feed.resize(limit);
        }
        return feed;
    }

private:
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

    unordered_map<string, User *> users_;
    unordered_map<string, Reel *> reels_;
    unordered_map<string, vector<string>> userToReels_;
    int reelCounter_ = 0;
};

} // namespace reel_platform_lld

#endif // INSTA_REEL_LLD_SERVICES_REELPLATFORMSERVICE_H

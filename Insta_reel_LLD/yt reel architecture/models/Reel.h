#ifndef INSTA_REEL_LLD_MODELS_REEL_H
#define INSTA_REEL_LLD_MODELS_REEL_H

#include <bits/stdc++.h>

#include "Comment.h"

using namespace std;

namespace reel_platform_lld {

class Reel {
public:
    Reel(string id, string ownerId, string caption, int durationSec)
        : id_(std::move(id)),
          ownerId_(std::move(ownerId)),
          caption_(std::move(caption)),
          durationSec_(durationSec),
          createdAt_(time(nullptr)),
          totalViews_(0),
          totalLikes_(0) {}

    const string &getId() const { return id_; }
    const string &getOwnerId() const { return ownerId_; }
    const string &getCaption() const { return caption_; }
    int getDurationSec() const { return durationSec_; }
    int getTotalViews() const { return totalViews_; }
    int getTotalLikes() const { return totalLikes_; }

    void addView(const string &viewerId) {
        if (viewerId.empty()) {
            return;
        }
        ++totalViews_;
        uniqueViewers_.insert(viewerId);
    }

    bool addLike(const string &userId) {
        if (likedBy_.count(userId) != 0) {
            return false;
        }
        likedBy_.insert(userId);
        ++totalLikes_;
        return true;
    }

    bool removeLike(const string &userId) {
        auto it = likedBy_.find(userId);
        if (it == likedBy_.end()) {
            return false;
        }
        likedBy_.erase(it);
        --totalLikes_;
        return true;
    }

    void addComment(const string &userId, const string &text) {
        comments_.push_back({userId, text, time(nullptr)});
    }

    void printStats() const {
        cout << "ReelId: " << id_ << "\n";
        cout << "Owner: " << ownerId_ << "\n";
        cout << "Caption: " << caption_ << "\n";
        cout << "Duration(sec): " << durationSec_ << "\n";
        cout << "Views: " << totalViews_ << " (Unique: " << uniqueViewers_.size() << ")\n";
        cout << "Likes: " << totalLikes_ << "\n";
        cout << "Comments: " << comments_.size() << "\n";
        cout << "-----------------------------\n";
    }

private:
    string id_;
    string ownerId_;
    string caption_;
    int durationSec_;
    long long createdAt_;
    int totalViews_;
    int totalLikes_;
    unordered_set<string> uniqueViewers_;
    unordered_set<string> likedBy_;
    vector<Comment> comments_;
};

} // namespace reel_platform_lld

#endif // INSTA_REEL_LLD_MODELS_REEL_H

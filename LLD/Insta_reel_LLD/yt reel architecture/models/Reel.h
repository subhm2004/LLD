// ============================================================================
//  models/Reel.h  —  Ek short video reel + uski poori engagement
// ----------------------------------------------------------------------------
//  Reel ke paas metadata (id, owner, caption, duration) aur engagement data
//  (views, likes, comments) dono hote hain. Ye ek "rich model" hai — data ke
//  saath uska behavior (addLike/removeLike/addView) bhi apne andar rakhta hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ TOTAL VIEWS vs UNIQUE VIEWERS — do alag metrics                       │
//  │                                                                          │
//  │     totalViews_      -> HAR view count hoti (ek banda 10 baar dekhe = 10) │
//  │     uniqueViewers_   -> kitne ALAG log ne dekha (set -> 10 baar = 1)      │
//  │                                                                          │
//  │  Dono alag business metrics hain: total views = "kitni baar chala"        │
//  │  (watch-time/ads ke liye), unique = "kitne logon tak pahuncha" (reach).   │
//  │  Isi liye counter AUR set dono rakhte hain — ek se doosra nahi milta.     │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ LIKES: `likedBy_` set se DUPLICATE-LIKE apne aap ruk jaata            │
//  │                                                                          │
//  │  addLike: user pehle se set me hai? -> false (kuch nahi hua)              │
//  │           nahi hai? -> set me daalo + count++ -> true                     │
//  │  removeLike: set me hai? -> hatao + count-- -> true, warna false          │
//  │                                                                          │
//  │  ⭐ Isi liye `totalLikes_` aur `likedBy_.size()` HAMESHA barabar rehte —  │
//  │  counter aur set ek saath update hote hain (kabhi out-of-sync nahi).      │
//  │  Bina set ke, ek banda 100 baar like dabaake count 100 kar deta. 😄        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  📌 Comments ko Reel OWN karta hai (`vector<Comment>` — composition). Reel
//     khatam = uske comments bhi khatam.
// ============================================================================
#ifndef INSTA_REEL_LLD_MODELS_REEL_H
#define INSTA_REEL_LLD_MODELS_REEL_H

#include <ctime>
#include <iostream>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

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
          createdAt_(time(nullptr)), // kab upload hui
          totalViews_(0),
          totalLikes_(0) {}

    const string &getId() const { return id_; }
    const string &getOwnerId() const { return ownerId_; } // feed me "kiska reel" check
    const string &getCaption() const { return caption_; }
    int getDurationSec() const { return durationSec_; }
    int getTotalViews() const { return totalViews_; } // feed ranking ka primary key
    int getTotalLikes() const { return totalLikes_; } // feed ranking ka tie-breaker

    // ---- VIEW: total count badhao + unique viewer record karo --------------
    void addView(const string &viewerId) {
        if (viewerId.empty()) {
            return; // invalid viewer — ignore
        }
        ++totalViews_;                  // har view ginti (repeat bhi)
        uniqueViewers_.insert(viewerId); // set -> alag log hi count
    }

    // ---- LIKE: duplicate-safe (set + counter saath me) --------------------
    bool addLike(const string &userId) {
        if (likedBy_.count(userId) != 0) {
            return false; // pehle se like kiya hua — kuch mat karo
        }
        likedBy_.insert(userId);
        ++totalLikes_;    // set aur counter HAMESHA saath (in-sync)
        return true;
    }

    bool removeLike(const string &userId) {
        auto it = likedBy_.find(userId);
        if (it == likedBy_.end()) {
            return false; // like kiya hi nahi tha
        }
        likedBy_.erase(it);
        --totalLikes_;
        return true;
    }

    // ---- COMMENT: reel apne comments own karta hai -------------------------
    void addComment(const string &userId, const string &text) {
        comments_.push_back({userId, text, time(nullptr)});
    }

    // Demo ke liye saari stats print.
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
    string id_;                             // "REEL_1"
    string ownerId_;                        // kisne upload kiya
    string caption_;
    int durationSec_;                       // short video ki lambai
    long long createdAt_;                   // upload time (epoch)
    int totalViews_;                        // har view (repeat include)
    int totalLikes_;                        // likedBy_.size() ke barabar rehta
    unordered_set<string> uniqueViewers_;   // kitne ALAG log ne dekha (reach)
    unordered_set<string> likedBy_;         // duplicate-like guard
    vector<Comment> comments_;              // reel apne comments own karta (composition)
};

} // namespace reel_platform_lld

#endif // INSTA_REEL_LLD_MODELS_REEL_H

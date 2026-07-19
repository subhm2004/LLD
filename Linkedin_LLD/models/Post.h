// ============================================================================
//  models/Post.h  —  Ek post + uspe hone wali saari engagement (R7, R8, R5)
// ----------------------------------------------------------------------------
//  R7 (Content Creation): "Users have the ability to create and share new posts."
//  R8 (Engagement):       "...interact with posts through REACTIONS, SHARES, and
//                          COMMENTS, and also engage with comments on posts."
//  R5 (Insights):         post ka "impressions" count (kitni baar feed me dikha).
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ PURANA Post sirf `int likeCount_` + `set<string> likedBy_` rakhta tha │
//  │                                                                          │
//  │  Do kami thi:                                                            │
//  │    1. Sirf "LIKE" — baaki 5 reactions (Celebrate/Love/...) the hi nahi   │
//  │    2. Comments aur shares ka concept tha hi nahi                         │
//  │                                                                          │
//  │  Ab:                                                                     │
//  │    reactions_ : map<userId, ReactionType>  -> 6 reactions, per user ek   │
//  │    comments_  : vector<Comment>            -> R8 comments                │
//  │    sharedBy_  : set<userId>                -> R8 shares (duplicate-safe)  │
//  │    impressions_ : int                      -> R5 analytics               │
//  │                                                                          │
//  │  💡 Backward compatible: `addLike()` / `getLikeCount()` abhi bhi hain —   │
//  │     andar se reactions_ pe kaam karte hain. Purana main.cpp tuta nahi.   │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ `map<userId, ReactionType>` kyun (sirf count nahi)?
//     - Ek user ka EK hi reaction rehta hai (map key unique) -> duplicate apne
//       aap block, aur user LIKE se LOVE me badal bhi sakta hai.
//     - Kaun-kaun ne react kiya, kis type se — sab pata (analytics/UI ke liye).
// ============================================================================
#ifndef LINKEDIN_LLD_MODELS_POST_H
#define LINKEDIN_LLD_MODELS_POST_H

#include <ctime>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "../enums/ReactionType.h"
#include "Comment.h"

namespace linkedin_lld {

class Post {
public:
    Post(std::string postId, std::string authorId, std::string content)
        : postId_(std::move(postId)),
          authorId_(std::move(authorId)),
          content_(std::move(content)),
          createdAt_(std::time(nullptr)) {}

    // ---- Core getters -------------------------------------------------------
    const std::string &getPostId() const { return postId_; }
    const std::string &getAuthorId() const { return authorId_; }
    const std::string &getContent() const { return content_; }
    long long getCreatedAt() const { return createdAt_; }

    // ========================================================================
    //  R8 — REACTIONS (6 types, per user ek)
    // ========================================================================
    // true  -> naya reaction ya type badla
    // false -> pehle se bilkul wahi reaction tha (kuch nahi hua)
    bool react(const std::string &userId, ReactionType type) {
        auto it = reactions_.find(userId);
        if (it != reactions_.end() && it->second == type) {
            return false;
        }
        reactions_[userId] = type;
        return true;
    }

    // ---- Backward-compatible "like" ----------------------------------------
    // Purana code `addLike(user)` bulata tha — wo ab LIKE reaction ban gaya.
    bool addLike(const std::string &userId) { return react(userId, ReactionType::LIKE); }

    // Total reactions (feed ranking isi engagement number pe hoti hai)
    int getReactionCount() const { return static_cast<int>(reactions_.size()); }

    // Sirf LIKE type ka count (backward-compatible naam, purane getLikeCount jaisa)
    int getLikeCount() const {
        int count = 0;
        for (const auto &entry : reactions_) {
            if (entry.second == ReactionType::LIKE) {
                ++count;
            }
        }
        return count;
    }

    // Reaction-type-wise breakdown (analytics/UI ke liye: kitne LIKE, kitne LOVE...)
    std::unordered_map<ReactionType, int> getReactionBreakdown() const {
        std::unordered_map<ReactionType, int> breakdown;
        for (const auto &entry : reactions_) {
            ++breakdown[entry.second];
        }
        return breakdown;
    }

    // ========================================================================
    //  R8 — COMMENTS (aur comments pe react — "engage with comments")
    // ========================================================================
    void addComment(const Comment &comment) { comments_.push_back(comment); }
    const std::vector<Comment> &getComments() const { return comments_; }

    // Comment ko id se dhoondho (react karne ke liye chahiye). Nahi mila -> nullptr.
    // Non-const version taaki us comment pe react() bulaya ja sake.
    Comment *findComment(const std::string &commentId) {
        for (Comment &c : comments_) {
            if (c.getCommentId() == commentId) {
                return &c;
            }
        }
        return nullptr;
    }

    // ========================================================================
    //  R8 — SHARES (kaun-kaun ne share kiya; duplicate share safe)
    // ========================================================================
    bool addShare(const std::string &userId) {
        return sharedBy_.insert(userId).second; // .second == false agar pehle se tha
    }
    int getShareCount() const { return static_cast<int>(sharedBy_.size()); }

    // ========================================================================
    //  R5 — IMPRESSIONS (post kitni baar kisi ke feed me dikha)
    // ========================================================================
    void incrementImpressions() { ++impressions_; }
    int getImpressions() const { return impressions_; }

private:
    std::string postId_;
    std::string authorId_;
    std::string content_;
    long long createdAt_;

    std::unordered_map<std::string, ReactionType> reactions_; // R8 (userId -> reaction)
    std::vector<Comment> comments_;                           // R8
    std::unordered_set<std::string> sharedBy_;                // R8 (who shared)
    int impressions_ = 0;                                     // R5
};

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_MODELS_POST_H

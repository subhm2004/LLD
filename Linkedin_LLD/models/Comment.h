// ============================================================================
//  models/Comment.h  —  Ek post pe kiya gaya comment (Requirement R8)
// ----------------------------------------------------------------------------
//  Requirement R8 (Engagement with Content): "...engage with COMMENTS on posts,
//  fostering a vibrant professional community."
//
//  ⭐ Ye line hi is class ki wajah hai: comment sirf text nahi hai — uspe khud
//  bhi REACT kiya ja sakta hai. Isi liye Comment ke paas apna reactions map hai,
//  bilkul Post ki tarah.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Comment aur Post kaafi milte-julte hain (dono pe react hota hai)      │
//  │                                                                          │
//  │  Sochne pe lagta hai "ek common base class bana do (Reactable)". Kar     │
//  │  sakte hain, par yahan jaan-boojh ke NAHI kiya — dono me abhi sirf       │
//  │  reactions common hai, aur early abstraction (galat jagah common code    │
//  │  nikaalna) aksar baad me dard deta hai. Zaroorat badhe to tab refactor.  │
//  │  📌 "Do baar dikhe to notice karo, teen baar dikhe to abstract karo."    │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef LINKEDIN_LLD_MODELS_COMMENT_H
#define LINKEDIN_LLD_MODELS_COMMENT_H

#include <ctime>
#include <string>
#include <unordered_map>
#include <utility>

#include "../enums/ReactionType.h"

namespace linkedin_lld {

class Comment {
public:
    Comment(std::string commentId, std::string authorId, std::string content)
        : commentId_(std::move(commentId)),
          authorId_(std::move(authorId)),
          content_(std::move(content)),
          createdAt_(std::time(nullptr)) {}

    const std::string &getCommentId() const { return commentId_; }
    const std::string &getAuthorId() const { return authorId_; }
    const std::string &getContent() const { return content_; }
    long long getCreatedAt() const { return createdAt_; }

    // ---- R8: comment pe react karo (like/celebrate/...) --------------------
    //
    // Ek user ka EK hi reaction (map key unique). Dobara wahi reaction -> false
    // (kuch nahi badla). Alag reaction -> update ho jaata hai, true.
    bool react(const std::string &userId, ReactionType type) {
        auto it = reactions_.find(userId);
        if (it != reactions_.end() && it->second == type) {
            return false; // pehle se yahi reaction hai
        }
        reactions_[userId] = type;
        return true;
    }

    int getReactionCount() const { return static_cast<int>(reactions_.size()); }

private:
    std::string commentId_;
    std::string authorId_;
    std::string content_;
    long long createdAt_;
    std::unordered_map<std::string, ReactionType> reactions_; // userId -> reaction (R8)
};

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_MODELS_COMMENT_H

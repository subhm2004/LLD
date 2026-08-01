// ============================================================================
//  enums/ReactionType.h  —  Post/Comment pe kis tarah ka reaction (Requirement R8)
// ----------------------------------------------------------------------------
//  Requirement R8 (Engagement with Content): "Users can interact with posts
//  through REACTIONS, shares, and comments..."
//
//  Purani LinkedIn me sirf "Like" tha. Ab 6 tarah ke reactions hote hain —
//  bilkul asli LinkedIn jaisa:
//
//     LIKE       👍  — normal pasand
//     CELEBRATE  🎉  — "badhaai ho!" (naya job, promotion)
//     SUPPORT    🤝  — "main saath hoon"
//     LOVE       ❤️  — "bahut acchha laga"
//     INSIGHTFUL  💡  — "kaafi kuch seekhne ko mila"
//     FUNNY      😄  — "mazedaar"
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ SIMPLE "like count" (int) se REACTION MAP (per-user) me kyun gaye?    │
//  │                                                                          │
//  │  Purana Post sirf `int likeCount_` rakhta tha. Do dikkat thi:            │
//  │    1. Ek user do baar like kar deta -> count galat (duplicate)           │
//  │    2. Sirf "like", baaki 5 reactions the hi nahi                         │
//  │                                                                          │
//  │  Ab Post ek `map<userId, ReactionType>` rakhta hai:                      │
//  │    - Ek user ka EK hi reaction (map key unique) -> duplicate apne aap    │
//  │      block, aur user reaction BADAL bhi sakta hai (LIKE -> LOVE)         │
//  │    - Har type ka alag count nikaalna easy                                │
//  │  (Post.h me poora explanation hai.)                                      │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  💡 Backward compatibility: purana `likePost(...)` abhi bhi chalta hai — wo
//     andar se `react(user, LIKE)` bula deta hai. Existing behavior tuta nahi.
// ============================================================================
#ifndef LINKEDIN_LLD_ENUMS_REACTIONTYPE_H
#define LINKEDIN_LLD_ENUMS_REACTIONTYPE_H

#include <string>

namespace linkedin_lld {

enum class ReactionType { LIKE, CELEBRATE, SUPPORT, LOVE, INSIGHTFUL, FUNNY };

// Reaction ko padhne-layak string me — feed/analytics printing ke liye.
inline std::string reactionTypeToString(ReactionType type) {
    switch (type) {
        case ReactionType::LIKE:       return "LIKE";
        case ReactionType::CELEBRATE:  return "CELEBRATE";
        case ReactionType::SUPPORT:    return "SUPPORT";
        case ReactionType::LOVE:       return "LOVE";
        case ReactionType::INSIGHTFUL: return "INSIGHTFUL";
        case ReactionType::FUNNY:      return "FUNNY";
    }
    return "UNKNOWN";
}

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_ENUMS_REACTIONTYPE_H

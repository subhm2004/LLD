// ============================================================================
//  services/FeedService.h  —  Personalized feed banane wali service (FR6)
// ----------------------------------------------------------------------------
//  FR6: "System should provide a personalized feed containing own and
//        connections' posts."
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ FEED LOGIC ko LinkedinSystem se ALAG rakha — kyun? (SRP + Strategy)   │
//  │                                                                          │
//  │  Feed "kaise banega" (kaunse posts, kis order me) ek badalne wala        │
//  │  faisla hai. Aaj: connections + apne posts, ranked by reactions phir     │
//  │  recency. Kal: ML-based ranking, ads beech me, "aapke interest" wale     │
//  │  posts... Ye poora logic ek ALAG service me hai, to LinkedinSystem ko    │
//  │  chhue bina ranking badli ja sakti hai. Yahi decoupling ka fayda.        │
//  └──────────────────────────────────────────────────────────────────────────┘
//
//  ⭐ RANKING: pehle ENGAGEMENT (total reactions), phir RECENCY (naya upar).
//     Purana code sirf `getLikeCount()` pe sort karta tha — ab
//     `getReactionCount()` (6 reactions ka total) pe, jo asli engagement hai.
//
//  ⭐ R5 tie-in: jo post feed me AATA hai uski "impression" count badha dete
//     hain — "post kitni baar kisi ke saamne aaya" (analytics ke liye).
//
//  💡 Post* (pointer) me kaam karte hain — asli posts LinkedinSystem ke paas
//     hain, ye service unhe sirf point karti hai (copy nahi banati). Isi liye
//     `const map<...>&` hote hue bhi post->incrementImpressions() chalta hai:
//     const to POINTER pe hai, jis Post pe point kar raha hai us pe nahi.
// ============================================================================
#ifndef LINKEDIN_LLD_SERVICES_FEEDSERVICE_H
#define LINKEDIN_LLD_SERVICES_FEEDSERVICE_H

#include <algorithm>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../models/Post.h"

namespace linkedin_lld {

class FeedService {
public:
    std::vector<Post *> buildFeed(const std::string &viewerId,
                                  const std::unordered_map<std::string, std::vector<Post *>> &postsByUser,
                                  const std::unordered_map<std::string, std::unordered_set<std::string>> &connections,
                                  size_t limit = 10) const {
        std::vector<Post *> feed;

        // 1. Connections ke posts daalo (accepted connections hi is map me aate hain)
        auto connIt = connections.find(viewerId);
        if (connIt != connections.end()) {
            for (const std::string &connectionId : connIt->second) {
                auto postIt = postsByUser.find(connectionId);
                if (postIt != postsByUser.end()) {
                    for (Post *post : postIt->second) {
                        feed.push_back(post);
                    }
                }
            }
        }

        // 2. Apne posts bhi daalo
        auto ownPostIt = postsByUser.find(viewerId);
        if (ownPostIt != postsByUser.end()) {
            for (Post *post : ownPostIt->second) {
                feed.push_back(post);
            }
        }

        // 3. Ranking: engagement (reactions) zyada -> upar; barabar ho to naya -> upar
        std::sort(feed.begin(), feed.end(), [](const Post *a, const Post *b) {
            if (a->getReactionCount() == b->getReactionCount()) {
                return a->getCreatedAt() > b->getCreatedAt(); // recency tie-breaker
            }
            return a->getReactionCount() > b->getReactionCount();
        });

        // 4. Limit tak kaato (pagination ka simple roop)
        if (feed.size() > limit) {
            feed.resize(limit);
        }

        // 5. R5: jo posts feed me dikhe unki impression count badhao
        for (Post *post : feed) {
            post->incrementImpressions();
        }
        return feed;
    }
};

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_SERVICES_FEEDSERVICE_H

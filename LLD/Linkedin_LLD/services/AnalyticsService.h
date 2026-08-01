// ============================================================================
//  services/AnalyticsService.h  —  User ke insights nikaalo (Requirement R5)
// ----------------------------------------------------------------------------
//  R5 (Insights and Analytics): "Users can view analytics related to their
//  CONNECTIONS, PROFILE VIEWS, POST IMPRESSIONS, and SEARCH APPEARANCES,
//  offering valuable feedback on their professional visibility."
//
//  Ye service khud koi data STORE nahi karti — data to models/other services ke
//  paas hai (profile views UserProfile me, impressions Post me, connections
//  graph me). Ye bas un sab ko PADH ke ek saaf summary bana ke deti hai.
//
//  ┌──────────────────────────────────────────────────────────────────────────┐
//  │  ⭐ Ye ek STATELESS "read-only aggregator" hai                           │
//  │                                                                          │
//  │  Iska koi member variable nahi. Sirf functions jo diye gaye data pe      │
//  │  hisaab lagate hain. Isi liye ise akele test karna trivial hai —         │
//  │  data do, answer lo, koi hidden state nahi. (FeedService/SearchService    │
//  │  bhi isi tarah stateless hain — ek consistent design choice.)            │
//  │                                                                          │
//  │  R5 ke 4 numbers 4 alag jagah se aate hain — ye service unhe EK JAGAH    │
//  │  jodti hai. Yahi "single source for insights" ka fayda.                  │
//  └──────────────────────────────────────────────────────────────────────────┘
// ============================================================================
#ifndef LINKEDIN_LLD_SERVICES_ANALYTICSSERVICE_H
#define LINKEDIN_LLD_SERVICES_ANALYTICSSERVICE_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../models/Post.h"
#include "../models/UserProfile.h"

namespace linkedin_lld {

class AnalyticsService {
public:
    // R5 ke chaaron number ek jagah.
    struct UserAnalytics {
        int connections = 0;       // kitne accepted connections
        int profileViews = 0;      // profile kitni baar dekha gaya
        int postImpressions = 0;   // saare posts ke impressions ka total
        int searchAppearances = 0; // search results me kitni baar aaya
    };

    UserAnalytics computeFor(const std::string &userId,
                             const std::unordered_map<std::string, UserProfile> &users,
                             const std::unordered_map<std::string, std::unordered_set<std::string>> &connections,
                             const std::unordered_map<std::string, std::vector<Post *>> &postsByUser) const {
        UserAnalytics analytics;

        // ---- Connections count (graph se) -----------------------------------
        auto connIt = connections.find(userId);
        if (connIt != connections.end()) {
            analytics.connections = static_cast<int>(connIt->second.size());
        }

        // ---- Profile views + search appearances (UserProfile se) ------------
        auto userIt = users.find(userId);
        if (userIt != users.end()) {
            analytics.profileViews = userIt->second.getProfileViews();
            analytics.searchAppearances = userIt->second.getSearchAppearances();
        }

        // ---- Post impressions (saare apne posts ka jod) ---------------------
        auto postIt = postsByUser.find(userId);
        if (postIt != postsByUser.end()) {
            for (const Post *post : postIt->second) {
                analytics.postImpressions += post->getImpressions();
            }
        }

        return analytics;
    }
};

} // namespace linkedin_lld

#endif // LINKEDIN_LLD_SERVICES_ANALYTICSSERVICE_H
